import re
import struct

DATA_START_ADDR = 0

TYPE_COUNT = 72033
TYPE_TABLE_ADDR = 0
CODE_GEN_MODULES_COUNT = 120
CODE_GEN_MODULES_ADDR = 0

TypeList = []
CodeGenModulesList = []
AddrMap = {}

def GetAddr(data):
    bs = data.split(' ')
    return ''.join(list(reversed(bs)))

def GetInt(data):
    bs = data.split(' ')
    byte_list = [int(byte, 16) for byte in bs[:4]]
    packed_bytes = struct.pack('<BBBB', *byte_list)
    #return struct.unpack('<i', packed_bytes)[0]
    return packed_bytes
    


if __name__ == '__main__':
    with open('binout.txt', 'r') as file:
        content = file.read()

    pattern = r'  ([0-9A-F]{16}): ((?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}))'
    matches = re.findall(pattern, content)

    DATA_START_ADDR = int(matches[0][0], 16)
    TYPE_TABLE_ADDR = int('0000000184710620', 16)
    '''CODE_GEN_MODULES_ADDR = int('0000000184BEEBA0', 16)'''

    type_table_start = (TYPE_TABLE_ADDR - DATA_START_ADDR) // 8
    type_table_end = type_table_start + TYPE_COUNT
    type_table = matches[type_table_start:type_table_end]

    '''
    code_gen_modules_start = (CODE_GEN_MODULES_ADDR - DATA_START_ADDR) // 8
    code_gen_modules_end = code_gen_modules_start + CODE_GEN_MODULES_COUNT
    code_gen_modules = matches[code_gen_modules_start:code_gen_modules_end]
    '''

    other_data = matches[:type_table_start] + matches[type_table_end:]

    for m in matches:
        AddrMap[m[0]] = m[1]
    for m in type_table:
        TypeList.append(m[0])
    '''
    for m in code_gen_modules:
        CodeGenModulesList.append(m[0])
    '''

    with open('types.bin', 'wb') as file:
        for t in TypeList:
            data_addr = GetAddr(AddrMap[t])
            if data_addr not in AddrMap.keys():
                print(f'error: {data_addr}')
                file.write(b'\xFF\xFF\xFF\xFF')
                continue
            data = GetInt(AddrMap[data_addr])
            file.write(data)
    '''
    with open('method-pointer.bin', 'wb') as file:
        index = 0
        file.write(CODE_GEN_MODULES_COUNT)
        index += 4
        for cgm in CodeGenModulesList:
            count = GetInt(AddrMap[cgm])

        pass
    '''