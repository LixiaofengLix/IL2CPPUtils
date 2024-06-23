import re
import struct
import binascii

DATA_START_ADDR = 0

TYPE_COUNT = 72033
TYPE_TABLE_ADDR = 0
CODE_GEN_MODULES_COUNT = 112
CODE_GEN_MODULES_ADDR = 0

TypeList = []
CodeGenModulesList = []
AddrMap = {}

BUFFER_OFFSET = 0
BUFFER = b''

def GetAddr(buf):
    addr = struct.unpack('<q', buf[:8])[0]
    return addr - BUFFER_OFFSET

def GetInt64(buf):
    return struct.unpack('<q', buf[:8])[0]

def GetString(buf):
    str_len = 0
    for b in buf:
        if b == 0:
            break
        str_len += 1
    name = struct.unpack(f'<{str_len}s', buf[:str_len])[0]
    return buf[:str_len + 1], name

if __name__ == '__main__':
    with open('binout.txt', 'r') as file:
        content = file.read()

    pattern = r'  ([0-9A-F]{16}): ([0-9A-F]{2}) ([0-9A-F]{2}) ([0-9A-F]{2}) ([0-9A-F]{2}) ([0-9A-F]{2}) ([0-9A-F]{2}) ([0-9A-F]{2}) ([0-9A-F]{2})'
    matches = re.findall(pattern, content)

    bs = []
    last = int(matches[0][0], 16) - 8
    for m in matches:
        # 补全中间缺失的数据
        if int(m[0], 16) - last > 8:
            bs += ['00'] * (int(m[0], 16) - last - 8)
        bs += m[1:]
        last = int(m[0], 16)
    BUFFER = bytes.fromhex(''.join(bs))
    BUFFER_OFFSET = int(matches[0][0], 16)

    CODE_GEN_MODULES_ADDR = int('0000000184BEEBA0', 16) - BUFFER_OFFSET

    method_pointers_header = {}

    name_buffer = b''
    name_map = {}

    data_buffer = b''
    data_map = {}

    method_pointers_data = {}
    for i in range(CODE_GEN_MODULES_COUNT):
        cgm = GetAddr(BUFFER[CODE_GEN_MODULES_ADDR + (i * 8):])

        p_module_name = GetAddr(BUFFER[cgm:])
        c_name, module_name = GetString(BUFFER[p_module_name:])
        name_map[module_name] = len(name_buffer)
        name_buffer += c_name

        method_pointer_count = GetInt64(BUFFER[cgm + 8:])
        method_pointers = GetAddr(BUFFER[cgm + 16:])
        data_map[module_name] = len(data_buffer)
        data_buffer += BUFFER[method_pointers:method_pointers + (method_pointer_count * 8)]

        method_pointers_header[module_name] = (name_map[module_name], method_pointer_count, data_map[module_name])
    
    with open('method-pointer.bin', 'wb') as file:
        file.write(struct.pack('<i', CODE_GEN_MODULES_COUNT))
        file.write(struct.pack('<i', 12 + (CODE_GEN_MODULES_COUNT * 12)))
        file.write(struct.pack('<i', 12 + (CODE_GEN_MODULES_COUNT * 12) + len(name_buffer)))
        for name, header in method_pointers_header.items():
            file.write(struct.pack('<i', header[0]))
            file.write(struct.pack('<i', header[1]))
            file.write(struct.pack('<i', header[2]))
        file.write(name_buffer)
        file.write(data_buffer)
