import sys
import struct
from PyQt6.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout, QPushButton,
    QListWidget, QLineEdit, QComboBox, QMessageBox, QFileDialog, QLabel, QTextEdit
)

# --- TGS Constants ---
MAGIC_BYTES = b'TGS'
DATA_START_OFFSET = 0x15
ENTRY_TERMINATOR = 0xFF
STRING_SEPARATOR = 0xFE
STRING_ARRAY_END = 0xFD

# Type codes
TYPE_CODES = {
    'Boolean': b'B1',
    'Int32': b'A0',
    'UInt32': b'A1',
    'Float': b'F0',
    'String': b'S0',
    'StringArray': b'S1'
}

class TgsEntry:
    def __init__(self, type_name, value, keyname):
        self.type_name = type_name
        self.value = value
        self.keyname = keyname[:20]  # max 20 chars

    def serialize(self):
        key_bytes = self.keyname.encode('ascii')
        if self.type_name == 'Boolean':
            val = b'\x02' if self.value else b'\x01'
        elif self.type_name == 'Int32':
            val = struct.pack('<i', int(self.value))
        elif self.type_name == 'UInt32':
            val = struct.pack('<I', int(self.value))
        elif self.type_name == 'Float':
            val = struct.pack('<f', float(self.value))
        elif self.type_name == 'String':
            val = self.value.encode('ascii')[:64]
        elif self.type_name == 'StringArray':
            val = b''
            for s in self.value[:200]:
                val += s.encode('ascii')[:64] + bytes([STRING_SEPARATOR])
            val += bytes([STRING_ARRAY_END])
        else:
            raise ValueError('Unknown type')
        return TYPE_CODES[self.type_name] + val + key_bytes + bytes([ENTRY_TERMINATOR])

class TgsEditor(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('TBag Save Editor (.tgs)')
        self.resize(700, 400)
        self.entries = []

        layout = QVBoxLayout()

        # Entry list
        self.list_widget = QListWidget()
        layout.addWidget(self.list_widget)

        # Controls
        controls = QHBoxLayout()

        self.type_combo = QComboBox()
        self.type_combo.addItems(TYPE_CODES.keys())
        controls.addWidget(QLabel('Type:'))
        controls.addWidget(self.type_combo)

        self.key_input = QLineEdit()
        self.key_input.setPlaceholderText('Key Name (max 20 chars)')
        controls.addWidget(QLabel('Key:'))
        controls.addWidget(self.key_input)

        self.value_input = QTextEdit()
        self.value_input.setPlaceholderText('Value (for arrays, separate lines)')
        self.value_input.setMaximumHeight(60)
        controls.addWidget(QLabel('Value:'))
        controls.addWidget(self.value_input)

        add_btn = QPushButton('Add/Update Entry')
        add_btn.clicked.connect(self.add_entry)
        controls.addWidget(add_btn)

        remove_btn = QPushButton('Remove Selected')
        remove_btn.clicked.connect(self.remove_entry)
        controls.addWidget(remove_btn)

        layout.addLayout(controls)

        # Save/Load
        file_controls = QHBoxLayout()
        load_btn = QPushButton('Load .tgs')
        load_btn.clicked.connect(self.load_tgs)
        save_btn = QPushButton('Save .tgs')
        save_btn.clicked.connect(self.save_tgs)
        file_controls.addWidget(load_btn)
        file_controls.addWidget(save_btn)

        layout.addLayout(file_controls)
        self.setLayout(layout)

    def add_entry(self):
        type_name = self.type_combo.currentText()
        keyname = self.key_input.text().strip()
        value_text = self.value_input.toPlainText().strip()

        if not keyname:
            QMessageBox.warning(self, 'Error', 'Key name cannot be empty')
            return

        if type_name == 'Boolean':
            value = value_text.lower() in ['true', '1']
        elif type_name in ['Int32', 'UInt32']:
            try:
                value = int(value_text)
            except:
                QMessageBox.warning(self, 'Error', 'Value must be an integer')
                return
        elif type_name == 'Float':
            try:
                value = float(value_text)
            except:
                QMessageBox.warning(self, 'Error', 'Value must be a float')
                return
        elif type_name == 'String':
            value = value_text
        elif type_name == 'StringArray':
            value = value_text.splitlines()
        else:
            return

        # Update existing or add
        for i, e in enumerate(self.entries):
            if e.keyname == keyname:
                self.entries[i] = TgsEntry(type_name, value, keyname)
                self.refresh_list()
                return

        self.entries.append(TgsEntry(type_name, value, keyname))
        self.refresh_list()

    def remove_entry(self):
        selected = self.list_widget.currentRow()
        if selected >= 0:
            self.entries.pop(selected)
            self.refresh_list()

    def refresh_list(self):
        self.list_widget.clear()
        for e in self.entries:
            display_val = e.value if not isinstance(e.value, list) else ','.join(e.value)
            self.list_widget.addItem(f'{e.keyname} ({e.type_name}) = {display_val}')

    def save_tgs(self):
        path, _ = QFileDialog.getSaveFileName(self, 'Save .tgs file', '', 'TBag Save (*.tgs)')
        if not path:
            return

        with open(path, 'wb') as f:
            f.write(MAGIC_BYTES)
            # Pad until offset 0x15
            f.write(b'\x00' * (DATA_START_OFFSET - len(MAGIC_BYTES)))
            # Write entries
            for e in self.entries:
                f.write(e.serialize())
        QMessageBox.information(self, 'Saved', f'Saved to {path}')

    def load_tgs(self):
        path, _ = QFileDialog.getOpenFileName(self, 'Open .tgs file', '', 'TBag Save (*.tgs)')
        if not path:
            return

        try:
            with open(path, 'rb') as f:
                data = f.read()
            if data[:3] != MAGIC_BYTES:
                QMessageBox.warning(self, 'Error', 'Invalid TGS file')
                return

            self.entries.clear()
            offset = DATA_START_OFFSET

            while offset < len(data):
                type_byte = data[offset:offset+2]
                offset += 2

                # Boolean
                if type_byte == TYPE_CODES['Boolean']:
                    value = data[offset] == 2
                    offset += 1
                elif type_byte == TYPE_CODES['Int32']:
                    value = struct.unpack('<i', data[offset:offset+4])[0]
                    offset += 4
                elif type_byte == TYPE_CODES['UInt32']:
                    value = struct.unpack('<I', data[offset:offset+4])[0]
                    offset += 4
                elif type_byte == TYPE_CODES['Float']:
                    value = struct.unpack('<f', data[offset:offset+4])[0]
                    offset += 4
                elif type_byte == TYPE_CODES['String']:
                    str_bytes = b''
                    while data[offset] != ENTRY_TERMINATOR:
                        str_bytes += bytes([data[offset]])
                        offset += 1
                    value = str_bytes.decode('ascii')
                elif type_byte == TYPE_CODES['StringArray']:
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
                else:
                    break

                # Read key until FF
                key_bytes = b''
                while data[offset] != ENTRY_TERMINATOR:
                    key_bytes += bytes([data[offset]])
                    offset += 1
                keyname = key_bytes.decode('ascii')
                offset += 1

                self.entries.append(TgsEntry(list(TYPE_CODES.keys())[list(TYPE_CODES.values()).index(type_byte)], value, keyname))

            self.refresh_list()

        except Exception as e:
            QMessageBox.warning(self, 'Error', f'Failed to load file: {e}')

if __name__ == '__main__':
    app = QApplication(sys.argv)
    editor = TgsEditor()
    editor.show()
    sys.exit(app.exec())
