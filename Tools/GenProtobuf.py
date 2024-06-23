import re

message_template = '''
message ##type## {
##fields##
}
'''


def load_type_from_metadata(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    types = content.split('------------------------------------------------------------------------------------------------------')[1:]
    return types
    # pattern = r'  ([0-9A-F]{16}): ((?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}) (?:[0-9A-F]{2}))'
    # matches = re.findall(pattern, content)

def create_protobuf(mtype, mfields):
    msg = message_template.replace('##type##', mtype[0])
    fields = ''
    for f in mfields:
        fields += f'\t{f[2]} {f[0]} = {f[1]};\n'
    msg = msg.replace('##fields##', fields)
    return msg

def get_protobuf_class(types):
    field_pattern = r'field\[\d+\] = \((?:\S+)\)(\S+)FieldNumber = (\d+)\n\s+field\[\d+\] = \((\S+)\)\1_'
    type_pattern = r'type\[\d+\] = (\S+), fieldCount = \d+,methodCount = \d+'

    protobufs = []
    for t in types:
        mfields = re.findall(field_pattern, t)
        if len(mfields) <= 0:
            continue
        mtype = re.findall(type_pattern, t)
        msg = create_protobuf(mtype, mfields)
        protobufs.append(msg)
    return protobufs

def main():
    types = load_type_from_metadata('D:\\CodeRepositories\\il2cpp\\IL2CPPUtils\\build\\out.txt')
    protobufs = get_protobuf_class(types)
    with open('jxsj4.proto', 'w') as file:
        file.write('syntax = "proto3";\n')
        for msg in protobufs:
            file.write(msg)

if __name__ == '__main__':
    main()