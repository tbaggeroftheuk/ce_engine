import struct

# TBag constants
MAGIC_BYTES = b'TGS'
DATA_START_OFFSET = 0x15
ENTRY_TERMINATOR = 0xFF
STRING_SEPARATOR = 0xFE
STRING_ARRAY_END = 0xFD

TYPE_CODES = {
    b'B1': 'Boolean',
    b'A0': 'Int32',
    b'A1': 'UInt32',
    b'F0': 'Float',
    b'S0': 'String',
    b'S1': 'StringArray'
}

def parse_tgs(filename):
    entries = []
    with open(filename, 'rb') as f:
        data = f.read()

    if data[:3] != MAGIC_BYTES:
        raise ValueError("Invalid TGS file")

    offset = DATA_START_OFFSET

    while offset < len(data):
        type_byte = data[offset:offset+2]
        offset += 2

        if type_byte not in TYPE_CODES:
            break
        type_name = TYPE_CODES[type_byte]

        # Read value based on type
        if type_name == 'Boolean':
            value = data[offset] == 2
            offset += 1
        elif type_name == 'Int32':
            value = struct.unpack('<i', data[offset:offset+4])[0]
            offset += 4
        elif type_name == 'UInt32':
            value = struct.unpack('<I', data[offset:offset+4])[0]
            offset += 4
        elif type_name == 'Float':
            value = struct.unpack('<f', data[offset:offset+4])[0]
            offset += 4
        elif type_name == 'String':
            str_bytes = b''
            while data[offset] != ENTRY_TERMINATOR:
                str_bytes += bytes([data[offset]])
                offset += 1
            value = str_bytes.decode('ascii')
        elif type_name == 'StringArray':
            arr = []
            str_bytes = b''
            while True:
                b = data[offset]
                offset += 1
                if b == STRING_SEPARATOR:
                    arr.append(str_bytes.decode('ascii'))
                    str_bytes = b''
                elif b == STRING_ARRAY_END:
                    arr.append(str_bytes.decode('ascii'))
                    break
                else:
                    str_bytes += bytes([b])
            value = arr

        # Read key name until FF
        key_bytes = b''
        while data[offset] != ENTRY_TERMINATOR:
            key_bytes += bytes([data[offset]])
            offset += 1
        keyname = key_bytes.decode('ascii')
        offset += 1

        entries.append((keyname, type_name, value))

    return entries

def pretty_print(entries):
    # Determine column widths
    key_width = max(len(k) for k,_,_ in entries) + 2
    type_width = max(len(t) for _,t,_ in entries) + 2

    # Header
    print(f"{'Key'.ljust(key_width)}{'Type'.ljust(type_width)}Value")
    print('-'*(key_width + type_width + 20))

    for k, t, v in entries:
        val_str = v if not isinstance(v, list) else ', '.join(v)
        print(f"{k.ljust(key_width)}{t.ljust(type_width)}{val_str}")

if __name__ == '__main__':
    import sys
    filename = sys.argv[1] if len(sys.argv) > 1 else 'garry.sav'
    entries = parse_tgs(filename)
    pretty_print(entries)
