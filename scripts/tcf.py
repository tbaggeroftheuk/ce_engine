import os
import struct
import zlib
import sys
from pathlib import Path

# -----------------------------
# Constants
# -----------------------------
MAGIC = b"TCF"
VERSION = 1
ENDIANNESS = b"\x00"
EOF_MARKER = b"EOF"
BUFFER_SIZE = 64 * 1024
SHIFT_BITS = 2

# -----------------------------
# Lookup tables for fast shift/unshift
# -----------------------------
SHIFT_TABLE = bytes(((i << SHIFT_BITS) & 0xFF) | (i >> (8 - SHIFT_BITS)) for i in range(256))
UNSHIFT_TABLE = bytes(((i >> SHIFT_BITS) & 0xFF) | ((i << (8 - SHIFT_BITS)) & 0xFF) for i in range(256))

def shift_data(data: bytes) -> bytes:
    return data.translate(SHIFT_TABLE)

def unshift_data(data: bytes) -> bytes:
    return data.translate(UNSHIFT_TABLE)

# -----------------------------
# Packing
# -----------------------------
def pack_tcf(folder: str, output_path: str) -> None:
    folder_path = Path(folder)
    if not folder_path.exists() or not folder_path.is_dir():
        raise ValueError("Invalid input folder")

    files_data = []
    total_original_size = 0

    for file_path in folder_path.rglob("*"):
        if file_path.is_file():
            rel_path = str(file_path.relative_to(folder_path)).replace("\\", "/")
            size = file_path.stat().st_size
            total_original_size += size
            files_data.append((rel_path, file_path, size))

    if not files_data:
        raise ValueError("No files found to pack")

    files_data.sort(key=lambda x: x[2])  # optional: sort by size

    output_path = Path(output_path)
    if output_path.suffix.lower() != ".tcf":
        output_path = output_path.with_suffix(".tcf")

    with open(output_path, "wb", buffering=BUFFER_SIZE) as out:
        # Reserve header space
        out.write(b"\x00" * (len(MAGIC) + 2 + 1 + 4 + 4 + 4))
        out.flush()

        offsets = []
        sizes = []
        paths = []
        current_offset = 0

        # Write payload
        for path, file_path, size in files_data:
            paths.append(path)
            offsets.append(current_offset)
            sizes.append(size)

            with open(file_path, "rb", buffering=BUFFER_SIZE) as f:
                while chunk := f.read(BUFFER_SIZE):
                    out.write(shift_data(chunk))

            current_offset += size

        payload_end = out.tell()

        # Write index
        for path, offset, size in zip(paths, offsets, sizes):
            path_bytes = path.encode("utf-8")
            out.write(struct.pack("<H", len(path_bytes)))
            out.write(path_bytes)
            out.write(struct.pack("<I", offset))
            out.write(struct.pack("<I", size))

        out.write(EOF_MARKER)

        # Write header + CRC
        out.seek(0)
        header_no_crc = (
            MAGIC +
            struct.pack("<H", VERSION) +
            ENDIANNESS +
            struct.pack("<I", payload_end) +
            struct.pack("<I", len(files_data))
        )
        crc = zlib.crc32(header_no_crc) & 0xFFFFFFFF
        out.write(header_no_crc)
        out.write(struct.pack("<I", crc))

    print(f"Packed {len(files_data)} files")
    print(f"Original size: {total_original_size} bytes")
    print(f"TCF size: {os.path.getsize(output_path)} bytes")

# -----------------------------
# Unpacking
# -----------------------------
def unpack_tcf(tcf_path: str, output_folder: str) -> None:
    with open(tcf_path, "rb") as f:
        data = f.read()

    pos = 0
    if data[pos:pos+3] != MAGIC:
        raise ValueError("Invalid magic")
    pos += 3

    version = struct.unpack("<H", data[pos:pos+2])[0]
    pos += 2
    pos += 1  # endianness byte

    index_offset = struct.unpack("<I", data[pos:pos+4])[0]
    pos += 4
    file_count = struct.unpack("<I", data[pos:pos+4])[0]
    pos += 4

    header_no_crc = data[:pos]
    crc_expected = struct.unpack("<I", data[pos:pos+4])[0]
    pos += 4

    if zlib.crc32(header_no_crc) & 0xFFFFFFFF != crc_expected:
        raise ValueError("CRC mismatch")

    payload = data[pos:index_offset]

    index_pos = index_offset
    entries = []
    for _ in range(file_count):
        l = struct.unpack("<H", data[index_pos:index_pos+2])[0]
        index_pos += 2
        path = data[index_pos:index_pos+l].decode("utf-8")
        index_pos += l
        offset = struct.unpack("<I", data[index_pos:index_pos+4])[0]
        index_pos += 4
        size = struct.unpack("<I", data[index_pos:index_pos+4])[0]
        index_pos += 4
        entries.append((path, offset, size))

    out_root = Path(output_folder)
    out_root.mkdir(parents=True, exist_ok=True)

    for path, offset, size in entries:
        out_path = out_root / path
        out_path.parent.mkdir(parents=True, exist_ok=True)
        raw = payload[offset:offset+size]
        with open(out_path, "wb") as f:
            f.write(unshift_data(raw))

    print(f"Extracted {len(entries)} files")

# -----------------------------
# View obfuscated bytes
# -----------------------------
def view_obfuscated(tcf_path: str, file_index: int = 0, num_bytes: int = 100) -> None:
    with open(tcf_path, "rb") as f:
        data = f.read()

    pos = 0
    if data[pos:pos+3] != MAGIC:
        raise ValueError("Invalid magic")
    pos += 3
    pos += 2
    pos += 1
    index_offset = struct.unpack("<I", data[pos:pos+4])[0]
    pos += 4
    file_count = struct.unpack("<I", data[pos:pos+4])[0]
    pos += 4
    pos += 4  # skip CRC

    index_pos = index_offset
    entries = []
    for _ in range(file_count):
        l = struct.unpack("<H", data[index_pos:index_pos+2])[0]
        index_pos += 2
        path = data[index_pos:index_pos+l].decode("utf-8")
        index_pos += l
        offset = struct.unpack("<I", data[index_pos:index_pos+4])[0]
        index_pos += 4
        size = struct.unpack("<I", data[index_pos:index_pos+4])[0]
        index_pos += 4
        entries.append((path, offset, size))

    if file_index >= len(entries):
        print("Index out of range")
        return

    path, offset, size = entries[file_index]
    payload_start = 18
    chunk = data[payload_start + offset:payload_start + offset + min(num_bytes, size)]

    print(path)
    print("Obfuscated:", chunk.hex())
    print("Unshifted :", unshift_data(chunk).hex())

# -----------------------------
# Command-line interface
# -----------------------------
if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage:")
        print("  pack <folder> <output.tcf>")
        print("  unpack <input.tcf> <output_folder>")
        print("  view <input.tcf> [index] [bytes]")
        sys.exit(1)

    cmd = sys.argv[1]

    if cmd == "pack":
        pack_tcf(sys.argv[2], sys.argv[3])
    elif cmd == "unpack":
        unpack_tcf(sys.argv[2], sys.argv[3])
    elif cmd == "view":
        idx = int(sys.argv[3]) if len(sys.argv) > 3 else 0
        n = int(sys.argv[4]) if len(sys.argv) > 4 else 100
        view_obfuscated(sys.argv[2], idx, n)
    else:
        print("Unknown command")
