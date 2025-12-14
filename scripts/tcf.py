import os
import struct
import zlib
import sys
import subprocess
import tempfile
import shutil
from pathlib import Path
from typing import List, Tuple

# -----------------------------
# Constants
# -----------------------------
MAGIC = b"TCF"
VERSION = 1
ENDIANNESS = b"\x00"   # little endian
EOF_MARKER = b"EOF"
BUFFER_SIZE = 64 * 1024  # 64KB buffer for reading/writing
SHIFT_BITS = 2  # Shift each byte by 2 bits (0-255 range)

# ==============================
# SHIFT/OBFUSCATION FUNCTIONS
# ==============================
def shift_byte(b: int) -> int:
    """Shift/obfuscate a single byte."""
    # Rotate bits left by SHIFT_BITS
    return ((b << SHIFT_BITS) & 0xFF) | (b >> (8 - SHIFT_BITS))

def unshift_byte(b: int) -> int:
    """Unshift/deobfuscate a single byte."""
    # Rotate bits right by SHIFT_BITS
    return ((b >> SHIFT_BITS) & 0xFF) | ((b << (8 - SHIFT_BITS)) & 0xFF)

def shift_data(data: bytes) -> bytes:
    """Shift/obfuscate a block of data."""
    return bytes(shift_byte(b) for b in data)

def unshift_data(data: bytes) -> bytes:
    """Unshift/deobfuscate a block of data."""
    return bytes(unshift_byte(b) for b in data)

# ==============================
# ZPAQ HELPER FUNCTIONS
# ==============================
def check_zpaq_available() -> bool:
    """Check if zpaq command is available."""
    try:
        result = subprocess.run(["zpaq", "version"], capture_output=True, text=True, check=False)
        return result.returncode == 0 or "zpaq" in result.stdout or "zpaq" in result.stderr
    except (subprocess.SubprocessError, FileNotFoundError):
        return False

def compress_with_zpaq(input_file: str, output_file: str) -> Tuple[bool, int, int]:
    """Compress a file using ZPAQ. Returns (success, original_size, compressed_size)."""
    original_size = os.path.getsize(input_file)
    
    try:
        print(f"Compressing with ZPAQ to '{output_file}'...")
        result = subprocess.run(
            ["zpaq", "a", output_file, input_file, "-m1"],
            capture_output=True,
            text=True,
            check=True
        )
        
        compressed_size = os.path.getsize(output_file)
        compression_ratio = (1 - compressed_size / original_size) * 100
        
        print(f"✓ ZPAQ compression successful")
        print(f"  Original: {original_size:,} bytes")
        print(f"  Compressed: {compressed_size:,} bytes")
        print(f"  Compression: {compression_ratio:.1f}% reduction")
        
        return True, original_size, compressed_size
    except subprocess.CalledProcessError as e:
        print(f"✗ ZPAQ compression failed: {e}")
        if e.stderr:
            print(f"  Error: {e.stderr.strip()}")
        return False, original_size, 0

def decompress_zpaq_to_file(input_file: str, output_file: str) -> str:
    """Decompress a ZPAQ archive that contains a single file."""
    try:
        print(f"Decompressing ZPAQ archive '{input_file}'...")
        
        # First, list contents to see what's inside
        result = subprocess.run(
            ["zpaq", "l", input_file],
            capture_output=True,
            text=True,
            check=True
        )
        
        # Extract to a temporary directory first
        temp_dir = tempfile.mkdtemp(prefix="tcf_zpaq_")
        
        # Extract everything
        result = subprocess.run(
            ["zpaq", "x", input_file, "-to", temp_dir],
            capture_output=True,
            text=True,
            check=True
        )
        
        # Find the extracted file(s)
        extracted_files = list(Path(temp_dir).rglob("*"))
        file_count = len([f for f in extracted_files if f.is_file()])
        
        if file_count == 0:
            raise ValueError("No files found in ZPAQ archive")
        elif file_count == 1:
            # Single file - find it and move it
            for file_path in extracted_files:
                if file_path.is_file():
                    shutil.move(str(file_path), output_file)
                    break
        else:
            # Multiple files - this shouldn't happen with our usage
            # But if it does, look for a .tcf file
            tcf_files = [f for f in extracted_files if f.is_file() and f.name.endswith('.tcf')]
            if tcf_files:
                shutil.move(str(tcf_files[0]), output_file)
            else:
                raise ValueError(f"Multiple files found in ZPAQ archive, expected single .tcf file")
        
        # Clean up temp directory
        shutil.rmtree(temp_dir)
        
        print(f"✓ ZPAQ decompression successful")
        return output_file
    except subprocess.CalledProcessError as e:
        print(f"✗ ZPAQ decompression failed: {e}")
        if e.stderr:
            print(f"  Error: {e.stderr.strip()}")
        raise

# ==============================
# PACKER
# ==============================
def pack_tcf(folder: str, output_path: str, compress: bool = False) -> None:
    """Pack a folder into a TCF archive with optional ZPAQ compression."""
    folder_path = Path(folder)
    files_data = []
    
    if not folder_path.exists():
        print(f"✗ Error: Folder '{folder}' does not exist")
        return
    
    if not folder_path.is_dir():
        print(f"✗ Error: '{folder}' is not a directory")
        return
    
    print(f"📁 Scanning folder: '{folder}'")
    
    # Gather all files
    total_original_size = 0
    for file_path in folder_path.rglob("*"):
        if file_path.is_file():
            rel_path = str(file_path.relative_to(folder_path)).replace("\\", "/")
            file_size = file_path.stat().st_size
            total_original_size += file_size
            files_data.append((rel_path, file_path, file_size))
    
    if not files_data:
        print("✗ No files found to pack")
        return
    
    # Sort files by size (smallest first for better compression)
    files_data.sort(key=lambda x: x[2])
    
    print(f"✓ Found {len(files_data)} files ({total_original_size:,} bytes total)")
    
    # Create TCF file
    tcf_file = output_path
    if compress and tcf_file.lower().endswith('.zpaq'):
        tcf_file = tcf_file[:-5]  # Remove .zpaq
    
    # Ensure .tcf extension
    if not tcf_file.lower().endswith('.tcf'):
        tcf_file += '.tcf'
    
    # Write TCF file
    print(f"📦 Creating TCF archive...")
    with open(tcf_file, "wb", buffering=BUFFER_SIZE) as out:
        # Write temporary header
        out.write(b"\x00" * (len(MAGIC) + 2 + 1 + 4 + 4 + 4))
        out.flush()
        
        # Write payload with byte shifting
        current_offset = 0
        path_entries = []
        offsets = []
        sizes = []
        
        processed_size = 0
        for i, (rel_path, file_path, file_size) in enumerate(files_data, 1):
            path_entries.append(rel_path)
            offsets.append(current_offset)
            sizes.append(file_size)
            
            # Read and shift file data in chunks
            with open(file_path, "rb", buffering=BUFFER_SIZE) as f:
                while chunk := f.read(BUFFER_SIZE):
                    shifted_chunk = shift_data(chunk)
                    out.write(shifted_chunk)
            
            current_offset += file_size
            processed_size += file_size
            
            if i % 10 == 0 or i == len(files_data):
                progress = (processed_size / total_original_size) * 100
                print(f"  Processed {i}/{len(files_data)} files ({progress:.1f}%)...", end='\r')
        
        payload_end = out.tell()
        
        # Write index
        for path, offset, size in zip(path_entries, offsets, sizes):
            path_bytes = path.encode("utf-8")
            out.write(struct.pack("<H", len(path_bytes)))
            out.write(path_bytes)
            out.write(struct.pack("<I", offset))
            out.write(struct.pack("<I", size))
        
        # Write EOF marker
        out.write(EOF_MARKER)
        
        # Go back and write correct header
        out.seek(0)
        
        # Build header without CRC
        header_no_crc = (
            MAGIC +
            struct.pack("<H", VERSION) +
            ENDIANNESS +
            struct.pack("<I", payload_end) +  # index offset
            struct.pack("<I", len(files_data))
        )
        
        # Calculate CRC32
        header_crc = zlib.crc32(header_no_crc) & 0xFFFFFFFF
        
        # Write final header
        out.write(header_no_crc)
        out.write(struct.pack("<I", header_crc))
    
    tcf_size = os.path.getsize(tcf_file)
    print(f"\n✓ Created TCF archive: '{tcf_file}' ({tcf_size:,} bytes)")
    
    # Apply ZPAQ compression if requested
    if compress:
        if not check_zpaq_available():
            print("\n✗ Error: zpaq command not available. Cannot compress.")
            print("  Install zpaq from: http://mattmahoney.net/dc/zpaq.html")
            return
        
        # Determine final output name
        if output_path.lower().endswith('.zpaq'):
            final_output = output_path
        else:
            final_output = output_path + '.zpaq'
        
        # Compress with zpaq
        success, _, compressed_size = compress_with_zpaq(tcf_file, final_output)
        
        if success:
            # Calculate overall compression ratio
            overall_ratio = (1 - compressed_size / total_original_size) * 100
            print(f"📊 Overall compression: {overall_ratio:.1f}% reduction")
            
            # Remove the temporary TCF file
            os.remove(tcf_file)
            print(f"✅ Final archive: '{final_output}'")
        else:
            # Compression failed, keep the TCF file
            print("\n⚠ Compression failed, keeping TCF file")
            print(f"✅ Saved as: '{tcf_file}'")
    else:
        print(f"✅ Final archive: '{tcf_file}'")
    
    print(f"\n📝 Summary:")
    print(f"  • Files packed: {len(files_data)}")
    print(f"  • Original size: {total_original_size:,} bytes")
    print(f"  • TCF size: {tcf_size:,} bytes")
    print(f"  • Data shifted by {SHIFT_BITS} bits for obfuscation")
    
    if compress and os.path.exists(final_output if 'final_output' in locals() else output_path):
        final_size = os.path.getsize(final_output if 'final_output' in locals() else output_path)
        print(f"  • Final size: {final_size:,} bytes")

# ==============================
# SIMPLE UNPACKER (with ZPAQ decompression support)
# ==============================
def unpack_tcf_simple(tcf_path: str, output_folder: str) -> None:
    """Simple unpacker with automatic ZPAQ decompression detection."""
    if not os.path.exists(tcf_path):
        print(f"✗ Error: Input file '{tcf_path}' not found")
        return
    
    print(f"📦 Opening archive: '{tcf_path}'")
    
    # Check if file is ZPAQ compressed
    is_zpaq_compressed = tcf_path.lower().endswith('.zpaq')
    
    # Decompress ZPAQ if needed
    if is_zpaq_compressed:
        if not check_zpaq_available():
            print("\n✗ Error: zpaq command not available. Cannot decompress.")
            print("  Install zpaq from: http://mattmahoney.net/dc/zpaq.html")
            return
        
        # Create temporary TCF file
        with tempfile.NamedTemporaryFile(delete=False, suffix='.tcf') as tmp:
            temp_tcf = tmp.name
        
        try:
            decompress_zpaq_to_file(tcf_path, temp_tcf)
            tcf_path = temp_tcf
            is_temp_file = True
        except Exception as e:
            print(f"✗ Failed to decompress: {e}")
            if os.path.exists(temp_tcf):
                os.remove(temp_tcf)
            return
    else:
        is_temp_file = False
    
    try:
        # Read the entire file
        print(f"📖 Reading TCF archive...")
        with open(tcf_path, "rb") as f:
            data = f.read()
        
        pos = 0
        # Parse header
        magic = data[pos:pos+3]; pos += 3
        if magic != MAGIC:
            raise ValueError("Invalid TCF magic")

        version = struct.unpack("<H", data[pos:pos+2])[0]; pos += 2
        endianness = data[pos]; pos += 1
        index_offset = struct.unpack("<I", data[pos:pos+4])[0]; pos += 4
        file_count = struct.unpack("<I", data[pos:pos+4])[0]; pos += 4

        header_no_crc = data[:pos]
        header_crc_expected = struct.unpack("<I", data[pos:pos+4])[0]; pos += 4

        header_crc_actual = zlib.crc32(header_no_crc) & 0xFFFFFFFF
        if header_crc_actual != header_crc_expected:
            raise ValueError(f"Header CRC32 mismatch: expected {header_crc_expected:08x}, got {header_crc_actual:08x}")

        # Payload starts after header
        payload_start = pos
        payload = data[payload_start:index_offset]

        # Parse index
        index_pos = index_offset
        entries = []
        for _ in range(file_count):
            path_len = struct.unpack("<H", data[index_pos:index_pos+2])[0]
            index_pos += 2
            path = data[index_pos:index_pos+path_len].decode("utf-8")
            index_pos += path_len
            offset = struct.unpack("<I", data[index_pos:index_pos+4])[0]
            index_pos += 4
            size = struct.unpack("<I", data[index_pos:index_pos+4])[0]
            index_pos += 4
            entries.append((path, offset, size))

        # Verify EOF marker
        eof = data[index_pos:index_pos+3]
        if eof != EOF_MARKER:
            print(f"⚠ Warning: EOF marker not found or archive may be truncated")
        
        # Create output directory
        output_path = Path(output_folder)
        output_path.mkdir(parents=True, exist_ok=True)
        
        # Extract files with de-shifting
        total_files = len(entries)
        extracted_count = 0
        total_size = 0
        
        print(f"📤 Extracting {total_files} files...")
        for i, (path, offset, size) in enumerate(entries, 1):
            out_path = output_path / path
            out_path.parent.mkdir(parents=True, exist_ok=True)
            
            # Get the shifted data
            shifted_data = payload[offset:offset+size]
            
            # De-shift the data
            unshifted_data = unshift_data(shifted_data)
            
            with open(out_path, "wb") as fp:
                fp.write(unshifted_data)
            
            extracted_count += 1
            total_size += size
            
            if i % 10 == 0 or i == total_files:
                progress = (i / total_files) * 100
                print(f"  Extracted {i}/{total_files} files ({progress:.1f}%)...", end='\r')
        
        print()  # New line after progress
        print(f"✅ Successfully extracted {extracted_count} files ({total_size:,} bytes) into '{output_folder}'")
        
    except Exception as e:
        print(f"\n✗ Error during extraction: {e}")
        raise
    finally:
        # Clean up temporary file if we created one
        if is_temp_file and os.path.exists(tcf_path):
            try:
                os.remove(tcf_path)
            except:
                pass

# ==============================
# VIEW OBFUSCATED CONTENT
# ==============================
def view_obfuscated(tcf_path: str, file_index: int = 0, num_bytes: int = 100) -> None:
    """View obfuscated content of a file in the archive."""
    if not os.path.exists(tcf_path):
        print(f"✗ Error: Input file '{tcf_path}' not found")
        return
    
    # Handle ZPAQ compressed files
    is_zpaq_compressed = tcf_path.lower().endswith('.zpaq')
    
    if is_zpaq_compressed:
        if not check_zpaq_available():
            print("✗ Error: zpaq command not available. Cannot view compressed file.")
            print("  Install zpaq from: http://mattmahoney.net/dc/zpaq.html")
            return
        
        print(f"📦 File is ZPAQ compressed. Extracting temporarily...")
        
        # Create temporary TCF file
        with tempfile.NamedTemporaryFile(delete=False, suffix='.tcf') as tmp:
            temp_tcf = tmp.name
        
        try:
            decompress_zpaq_to_file(tcf_path, temp_tcf)
            tcf_path = temp_tcf
            is_temp_file = True
        except Exception as e:
            print(f"✗ Failed to decompress: {e}")
            if os.path.exists(temp_tcf):
                os.remove(temp_tcf)
            return
    else:
        is_temp_file = False
    
    try:
        with open(tcf_path, "rb") as f:
            data = f.read()
        
        pos = 0
        magic = data[pos:pos+3]; pos += 3
        if magic != MAGIC:
            raise ValueError("Invalid TCF magic")

        version = struct.unpack("<H", data[pos:pos+2])[0]; pos += 2
        endianness = data[pos]; pos += 1
        index_offset = struct.unpack("<I", data[pos:pos+4])[0]; pos += 4
        file_count = struct.unpack("<I", data[pos:pos+4])[0]; pos += 4
        header_no_crc = data[:pos]
        header_crc_expected = struct.unpack("<I", data[pos:pos+4])[0]; pos += 4

        # Parse index
        index_pos = index_offset
        entries = []
        for i in range(file_count):
            path_len = struct.unpack("<H", data[index_pos:index_pos+2])[0]
            index_pos += 2
            path = data[index_pos:index_pos+path_len].decode("utf-8")
            index_pos += path_len
            offset = struct.unpack("<I", data[index_pos:index_pos+4])[0]
            index_pos += 4
            size = struct.unpack("<I", data[index_pos:index_pos+4])[0]
            index_pos += 4
            entries.append((path, offset, size))
        
        if file_index >= len(entries):
            print(f"✗ File index {file_index} out of range (0-{len(entries)-1})")
            return
        
        path, offset, size = entries[file_index]
        payload_start = 18  # Header size
        
        # Get the shifted data
        bytes_to_show = min(num_bytes, size)
        shifted_data = data[payload_start + offset:payload_start + offset + bytes_to_show]
        
        print(f"\n📄 File: {path}")
        print(f"📏 Size: {size:,} bytes")
        print(f"🔍 Showing first {len(shifted_data)} bytes")
        
        print(f"\n🎭 Obfuscated (shifted by {SHIFT_BITS} bits):")
        print(f"🔢 Hex: {shifted_data.hex()}")
        print(f"🔤 ASCII: {shifted_data}")
        
        # Show de-shifted
        unshifted_data = unshift_data(shifted_data)
        print(f"\n✨ De-obfuscated:")
        print(f"🔢 Hex: {unshifted_data.hex()}")
        
        try:
            ascii_repr = unshifted_data.decode('utf-8', errors='replace')
            print(f"🔤 ASCII: {ascii_repr}")
        except:
            print(f"🔤 ASCII: [Binary data, cannot decode as UTF-8]")
        
    finally:
        # Clean up temporary file if we created one
        if is_temp_file and os.path.exists(tcf_path):
            try:
                os.remove(tcf_path)
            except:
                pass

# ==============================
# CLI
# ==============================
if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("🔧 TCF Archive Tool with ZPAQ Compression Support")
        print("=" * 50)
        print("Usage:")
        print("  python tcf.py pack <folder> <output.tcf> [-z|--zpaq]")
        print("  python tcf.py unpack <input.tcf> <output_folder>")
        print("  python tcf.py view <input.tcf> [file_index] [num_bytes]")
        print("\nOptions:")
        print("  -z, --zpaq    Compress the TCF archive with ZPAQ")
        print("\nExamples:")
        print("  python tcf.py pack myfolder archive.tcf")
        print("  python tcf.py pack myfolder archive.tcf.zpaq -z")
        print("  python tcf.py unpack archive.tcf.zpaq extracted")
        print("  python tcf.py view archive.tcf 0 50")
        
        # Only check ZPAQ if we're going to show the warning
        if len(sys.argv) == 2 and sys.argv[1] in ['help', '--help', '-h']:
            if not check_zpaq_available():
                print("\nℹ Note: zpaq command not found. Install from:")
                print("http://mattmahoney.net/dc/zpaq.html")
        sys.exit(1)

    mode = sys.argv[1].lower()
    
    try:
        if mode == "pack":
            folder = sys.argv[2]
            output = sys.argv[3]
            compress = False
            
            # Check for compression flag
            for arg in sys.argv[4:]:
                if arg in ['-z', '--zpaq']:
                    compress = True
                    break
            
            pack_tcf(folder, output, compress)
            
        elif mode == "unpack":
            input_file = sys.argv[2]
            output_folder = sys.argv[3]
            
            unpack_tcf_simple(input_file, output_folder)
            
        elif mode == "view":
            input_file = sys.argv[2]
            file_index = 0
            num_bytes = 100
            
            if len(sys.argv) > 3:
                try:
                    file_index = int(sys.argv[3])
                except ValueError:
                    print(f"✗ Error: Invalid file index '{sys.argv[3]}'")
                    sys.exit(1)
                    
            if len(sys.argv) > 4:
                try:
                    num_bytes = int(sys.argv[4])
                except ValueError:
                    print(f"✗ Error: Invalid byte count '{sys.argv[4]}'")
                    sys.exit(1)
                    
            view_obfuscated(input_file, file_index, num_bytes)
            
        else:
            print(f"✗ Unknown command: {mode}")
            print("Available commands: pack, unpack, view")
            sys.exit(1)
    except Exception as e:
        print(f"✗ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)