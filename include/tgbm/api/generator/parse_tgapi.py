import os
import requests
from bs4 import BeautifulSoup
import argparse

K_ONE_OF = 'oneof'
K_ONE_OF_FIELD = 'oneof_field'
K_DEFAULT = 'default'
K_MANUALLY = 'manually'

K_ONE_OF_FIELD_TYPES = ['Update', 'KeyboardButton', 'InlineKeyboardButton']

def path_api_html(apidir):
    return f'{apidir}/generator/tgapi.html'

def path_types_file(apidir):
    return f'{apidir}/generator/types.txt'

def path_methods_file(apidir):
    return f'{apidir}/generator/methods.txt'

def manually_type(apidir, type_name):
    return os.path.exists(f'{apidir}/def/{type_name}.nodef')

def get_elem_field_type(raw_field_type: str):
    types = {
        "IntegerorString": "IntOrStr",
        "InputFileorString": "FileOrStr"
    }
    if raw_field_type in types.keys():
        return types[raw_field_type]
    else:
        return raw_field_type


def get_real_field_type(infos, raw_field_type: str):
    raw_field_type = raw_field_type.strip()
    raw_field_type = raw_field_type.replace(" ", "")
    types = {
        "Integer": "INTEGER",
        "True": "TRUE_FIELD",
        "Boolean": "BOOLEAN",
        "String": "STRING",
        "IntegerorString": "INT_OR_STR",
        "InputFileorString": "FILE_OR_STR",
        "Float": "FLOAT"
    }

    if raw_field_type.startswith("ArrayofArrayof"):
        return  f"ARRAYOF_ARRAYOF, {get_elem_field_type(raw_field_type[14:])}"
    elif raw_field_type.startswith("Arrayof"):
        return  f"ARRAYOF, {get_elem_field_type(raw_field_type[7:])}"
    
    
    if raw_field_type in types.keys():
        return types[raw_field_type]
    elif raw_field_type in infos.keys() and infos[raw_field_type]['type'] == K_ONE_OF:
        return f'INLINED, {raw_field_type}'
    else:
        return f'COMPOUND, {raw_field_type}'

def convert_type_to_name_field(type_name):
    result = ""
    i = 0
    while i < len(type_name):
        letter = type_name[i]
        if letter.isupper():
            if i > 0: result += "_"
            result += letter.lower()
        else:
            result += letter
        i+=1
    return result

def parse_info_one_of(parsed_api, type_name, type_header, type_description, table_one_of):
    result = {
        'description': type_description.text,
        'type': K_ONE_OF,
        'fields': {}
    }
    index = 0
    for one_of_type in table_one_of.find_all('li'):
        a = one_of_type.find('a')
        one_of_type_s = a.text
        name_field = convert_type_to_name_field(one_of_type_s)
        result['fields'][name_field] = {
            'type': one_of_type_s,
            'index': index
        }
        index += 1
    return result

def get_default_type(type_name):
    if type_name in K_ONE_OF_FIELD_TYPES:
        return K_ONE_OF_FIELD
    else:
        return K_DEFAULT

def parse_info_default(parsed_api, type_name, type_header, type_description):
    result = {
        'description': type_description.text,
        'type': get_default_type(type_name),
        'fields': {}
    }
    table = type_header.find_next('table', {'class': 'table'})
    if not table:
        print(f"No table found for type {type_name}")
        return
    #TODO: здесь надо брать следующую ноду и проверять, что это нужный тип
    #TODO: тогда генерация не будет брать хрен пойми чужую таблицу для каких-нибудь oneof
    rows = table.find_all('tr')[1:]  # Пропускаем заголовок таблицы  
    index = 0
    for row in rows:
        columns = row.find_all('td')
        if len(columns) >= 3:
            field_name = columns[0].get_text(strip=True)
            field_type = columns[1].get_text(strip=True)
            field_description : str = columns[2].get_text(strip=True)
            result['fields'][field_name] = {
                'type': field_type,
                'description': field_description,
                'raw_description': columns[2].get_text(separator=' '),
                'index': index
            }
            index+=1
    return result

def parse_info(type_name, parsed_api):
    type_header = parsed_api.find('a', {'name': type_name.lower()})
    if not type_header:
        print(f"Type {type_name} not found on the page.")
        return
    type_description = type_header.find_next('p')
    table_one_of = type_description.find_next_sibling()

    if 'one of' in str(type_description).strip() and table_one_of and table_one_of.name == 'ul':
        return parse_info_one_of(parsed_api, type_name, type_header, type_description, table_one_of)
    else:
        return parse_info_default(parsed_api, type_name, type_header, type_description)


def get_all_api(apidir, is_classes, is_methods):
    with open(path_api_html(apidir), 'r', encoding='utf-8') as file:
        html_content = file.read()
    soup = BeautifulSoup(html_content, 'html.parser')
    # Поиск всех заголовков <h4> и разделение их на классы и методы
    classes = []
    methods = []

    for tag in soup.find_all('h4'):
        text = tag.get_text(strip=True)
        if text and len(text.split()) == 1:
            if text[0].isupper():
                classes.append(text)
            elif text[0].islower():
                methods.append(text)
    
    if is_classes:
        with open(path_types_file(apidir), 'w', encoding='utf-8') as file:
            for _class in classes:
                file.write(f'{_class}\n')

    if is_methods:
        with open(path_types_file(apidir), 'w', encoding='utf-8') as file:
            for method in methods:
                file.write(f'{method}\n')

def load_api(outfile):
    url = "https://core.telegram.org/bots/api"

    response = requests.get(url)
    response.raise_for_status()
    with open(outfile, 'w', encoding='utf-8') as out:
        print(response.text, file=out)
    print(f"API stored into {outfile}")

def generate_default_type(infos, type_name, info, file):
    for field_name, field_info in info['fields'].items():
        field_type = field_info['type']
        field_description = field_info['description']
        if field_description.startswith('Optional'):
            size = field_description.find(' ')
            while field_description[size].isspace():
                size += 1
            file.write("OPTIONAL(")
        else:
            file.write("REQUIRED(")
        file.write(f"{get_real_field_type(infos, field_type)}, {field_name})\n")     

def generate_oneof_field_type(infos, type_name, info, file):
    for field_name, field_info in info['fields'].items():
        field_type = field_info['type']
        field_description = field_info['description']
        if field_description.startswith('Optional'):
            size = field_description.find(' ')
            while field_description[size].isspace():
                size += 1
            file.write("OPTIONAL(")
        else:
            file.write("REQUIRED(")
        file.write(f"{get_real_field_type(infos, field_type)}, {field_name})\n")      

def generate_one_of_type(type_name, info, file):
    file.write(f"DISCRIMINATOR_FIELD({info['discriminator']})\n")
    for discriminator_type_name, discriminator_value in info['discriminator_values'].items():
        file.write(f'DISCRIMINATOR_VALUE({discriminator_type_name}, {discriminator_value})\n')
    for name_field, field_info in info['fields'].items():
        type_field = field_info['type']
        file.write(f"VARIANT_FIELD(COMPOUND, {type_field}, {name_field})\n")

def generate_def_impl(infos, type_name, info, apidir):
    # Ищем заголовок, соответствующий нужному типу
    type = info['type']

    file_path = f'{apidir}/def/{type}/{type_name}.def'
    os.makedirs(os.path.dirname(file_path), exist_ok=True)
    with open(file_path, 'w', encoding='utf-8') as file:
        file.write("#include \"tgbm/api/utils/begin.h\"\n\n")
        if type == K_ONE_OF:
            generate_one_of_type(type_name, info, file)
        elif type == K_DEFAULT or type == K_ONE_OF_FIELD:
            generate_default_type(infos, type_name, info, file)
        else:
            print(f'unexpected type: {type}')
            assert(False)
        file.write("\n#include \"tgbm/api/utils/end.h\"\n")
    print(f"Data for type {type_name} has been saved to {apidir}/def/{type_name}.def")

def found_first_field(info):
    for field_name, field_info in info['fields'].items():
        if field_info['index'] == 0:
            return field_name
    return None

def process_oneof(infos, type_name, type_info):
    fields_info = {}
    for field_name, field_info in type_info['fields'].items():
        field_type = field_info['type']
        type_field_info = infos[field_type]
        
        if 'is_sub_one_of' in type_field_info.keys():
            continue
        type_field_info['is_sub_one_of'] = True
        discriminator = found_first_field(type_field_info)
        type_field_info['discriminator'] = discriminator
        discriminator_field = type_field_info['fields'][discriminator]
        discriminator_value = discriminator_field['raw_description'].split()[-1].replace('”', '').replace('“', '')
        print(f'discriminator_value: [{discriminator_value}]')
        type_field_info['discriminator_value'] = discriminator_value
        type_field_info['fields'].pop(discriminator)
        if 'discriminator' not in type_info.keys():
            type_info['discriminator'] = discriminator
        else:
            type_info_discriminator = type_info['discriminator']
            assert type_info_discriminator == discriminator, f'type_name: {type_name}, type_info_discriminator: {type_info_discriminator}, discriminator: {discriminator}'
        if 'discriminator_values' not in type_info.keys():
            type_info['discriminator_values'] = {}
        type_info['discriminator_values'][field_type] = discriminator_value
        fields_info[discriminator_value] = field_info

    type_info['fields'] = fields_info   

def rewrite_sub_oneof(infos):
    for type_name, type_info in infos.items():
        if type_info['type'] == 'oneof':
            process_oneof(infos, type_name, type_info)

def get_all_infos(parsed_api, apidir):
    typelist = open(f'{apidir}/generator/types.txt', 'r', encoding='utf-8')
    infos = {}
    for type_name in typelist:
        type_name = type_name.strip()
        if not manually_type(apidir, type_name):
            infos[type_name] = parse_info(type_name, parsed_api)
        else:
            infos[type_name] = {
                'type': K_MANUALLY
            }

    rewrite_sub_oneof(infos)
    return infos

def generate_defs_impl(apidir, typelist=None):
    with open(f'{apidir}/generator/tgapi.html', 'r', encoding='utf-8') as file:
        content = file.read()
        parsed_api = BeautifulSoup(content, 'html.parser')
    infos = {}
    if typelist is None:
        typelist = open(f'{apidir}/generator/types.txt', 'r', encoding='utf-8')
    for type_name in typelist:
        type_name = type_name.strip()
        if not manually_type(apidir, type_name):
            infos[type_name] = parse_info(type_name, parsed_api)

    rewrite_sub_oneof(infos)
    for type_name, info in infos.items():
        generate_def_impl(infos, type_name, info, apidir)


def postprocess_hpp(file_path, infos, fwd_file, all_file, apidir):
    type_name, _ = os.path.splitext(os.path.basename(file_path))
    if manually_type(apidir, type_name):
        return
    with open(file_path, 'r') as file:
        contents = file.read()
    fwd_file.write(f'  struct {type_name};\n')
    all_file.write(f'#include "tgbm/api/types/{type_name}.hpp"\n')
    info = infos[type_name]

    file = open(f'{file_path}', 'w')
    file.write(f'#pragma once\n#include "tgbm/api/common.hpp"\n#include "tgbm/api/types_fwd.hpp"\n')
    for field_name, field_info in info['fields'].items():
        field_type = field_info['type']
        if not field_type in infos.keys():
            continue 
        if infos[type_name]['type'] == K_ONE_OF_FIELD:
            file.write(f'#include "tgbm/api/types/{field_type}.hpp"\n')
            continue
        if infos[field_type]['type'] == K_ONE_OF or infos[field_type]['type'] == K_ONE_OF_FIELD:
            file.write(f'#include "tgbm/api/types/{field_type}.hpp"\n') 
    file.write('\n\n')
    for line in contents.splitlines():
        if f'struct {type_name}' in line:
            description = str(info['description']).replace('\n', ' ')
            file.write(f'//{description}\n{line}\n')
            continue

        for field_name, field_info in info['fields'].items():
            if 'description' in field_info:
                if f'{field_name};' in line:
                    description = str(field_info['description']).replace('\n', ' ')
                    file.write(f'  // {description}\n')
                    break
        file.write(f'{line}\n')


def postprocess(apidir):
    with open(f'{apidir}/generator/tgapi.html', 'r', encoding='utf-8') as file:
        content = file.read()
        parsed_api = BeautifulSoup(content, 'html.parser')

    infos = get_all_infos(parsed_api, apidir)
    
    fwd_file = open(f'{apidir}/types_fwd.hpp', 'w', encoding='utf-8')
    fwd_file.write('#pragma once\n\n')
    fwd_file.write('namespace tgbm::api{\n')

    all_file = open(f'{apidir}/types_all.hpp', 'w', encoding='utf-8')
    all_file.write('#pragma once\n\n')
    
    for root, _, files in os.walk(f'{apidir}/types'):
        for file in files:
            if file.endswith('.hpp'):
                file_path = os.path.join(root, file)
                postprocess_hpp(file_path, infos, fwd_file, all_file, apidir)
    
    fwd_file.write('}\n')

def main():
    parser = argparse.ArgumentParser(description='Extract fields and descriptions for a given type from the Telegram Bot API.')
    parser.add_argument('--generate_defs', action='store_true', help='Generate .def files')
    parser.add_argument('--apidir', type=str, help="path to /api folder in project", default='.')
    parser.add_argument('--get_methods', action='store_true', help="gets all api methods and exits")
    parser.add_argument('--get_classes', action='store_true', help="gets all api classes and exits")
    parser.add_argument('--load_api', type=str, help="loads from internet TG API to file, usage: --load_api=<path>")
    parser.add_argument('--postprocess', action='store_true', help="post preprocess logic: add descriptions of fields and methods, add necessary includes")
    parser.add_argument('--typelist', nargs='+', type=str, help='list of types to generate def. If not present, use types from api/types.txt')
    args = parser.parse_args()
    if args.postprocess:
        postprocess(args.apidir)
        return
    if args.get_methods or args.get_classes:
        get_all_api(args.apidir, args.get_classes, args.get_methods)
        return
    if args.load_api:
        load_api(args.load_api)
        return
    if args.generate_defs:
        generate_defs_impl(args.apidir, args.typelist)
        return
    parser.print_usage()

if __name__ == '__main__':
    main()
