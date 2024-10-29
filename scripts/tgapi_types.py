from bs4 import BeautifulSoup
import requests
import sys
import typing
import argparse
import os
import re
import json

# loads TG api (html)
def load_api_html():
    url = "https://core.telegram.org/bots/api"
    response = requests.get(url)
    if response.status_code != 200:
        print("[ERROR]: generating methods, cannot load TG API", response.status_code)
        return []
    return response.text

######################### PARSING ########################

G_TYPE_MAPPING = {
    "integer": "Integer",
    "string": "String",
    "integerorstring": "int_or_str",
    "boolean": "bool",
    "true": 'True',
    "float": "Double",
    "inputfileorstring": 'file_or_str',
    "inputfile": "InputFile",
}

PARSED_TYPES = {}

def map_tgtype_to_cpptype(tgtype: str):
    typeunified = tgtype.strip().lower()
    typeunified = typeunified.replace(' ', '').replace('\n', '')
    if typeunified in G_TYPE_MAPPING:
        return G_TYPE_MAPPING[typeunified]
    if typeunified.startswith('arrayof'):
        return f'arrayof<{map_tgtype_to_cpptype(typeunified[len("arrayof"):])}>'
    return tgtype # unmodified, just complex type

class field_info_t:
    def __init__(self, name: str, tgtype: str, is_optional: bool, description: str):
        self.name = name
        self.tgtype = tgtype
        self.is_optional = is_optional
        self.cpptype = map_tgtype_to_cpptype(tgtype)
        self.description = description
    
    def to_dict(self):
        return {
            "name": self.name,
            "tgtype": self.tgtype,
            "is_optional": self.is_optional,
            "cpptype": self.cpptype,
            "description": self.description,
        }

class type_info_t:
    def __init__(self, name: str, description: str, fields: list[field_info_t]):
        self.name = name
        self.fields: list[field_info_t] = fields
        self.description = description
        K_ONEOFS = ['Update', 'KeyboardButton', 'InlineKeyboardButton', 'InlineQueryResultsButton']
        self.is_merged_optional_fields = name in K_ONEOFS
    
    def to_dict(self):
        return {
            "name": self.name,
            "description": self.description,
            "is_merged_optional_fields": self.is_merged_optional_fields,
            "fields": self.fields,
        }

def parse_discriminator_value(alternative_description: str)->str:
    d = alternative_description
    d = d.replace('\n', ' ').strip()
    # NOTE: special quotes, not regular (telegram api..)
    match = re.search('.*always\s+“(.+)”$', d)
    if match:
        return match.group(1)
    match = re.search('.*must\s+be\s+(.+)$', d)
    if match:
        return match.group(1)
    raise ValueError()

TYPES_WITHOUT_DISCRIMINATOR = {
    'InputMessageContent'
}
# represents one possible state of oneof
class oneof_alternative_t:
    def __init__(self, name: str, tgtype: type_info_t, description: str, owner_type: str):
        self.name = name
        self.tgtype = tgtype
        self.discriminator_value=parse_discriminator_value(tgtype.fields[0].description) if owner_type not in TYPES_WITHOUT_DISCRIMINATOR else None
        self.description = description

        self.tgtype.fields = self.tgtype.fields[1:]

    def to_dict(self):
        return {
            "name": self.name,
            "discriminator_value": self.discriminator_value if self.discriminator_value is not None else "<NOTHING>",
            "description": self.description,
            "tgtype": self.tgtype,
        }

class oneof_info_t:
    def __init__(self, name: str, discriminator_name: str, description: str, alternatives: list[oneof_alternative_t]):
        self.name = name
        self.discriminator_name = discriminator_name
        self.alternatives: list[oneof_alternative_t] = alternatives
        self.description = description

    def to_dict(self):
        return {
            "name": self.name,
            "discriminator_name": self.discriminator_name,
            "alternatives": self.alternatives,
            "description": self.description
        }

# accepts type name, description and HTML node of table of fields
def parse_compound_type(name: str, description: str, fieldtable) -> type_info_t:
    fields : list[field_info_t] = []
    for row in fieldtable.find_all('tr')[1:]:
        cells = row.find_all('td')
        fields.append(field_info_t(
            name=cells[0].text.strip(),
            tgtype=cells[1].text.strip(),
            is_optional=cells[2].text.replace('\n', ' ').strip().startswith('Optional'),
            description=cells[2].text.replace('\n', ' ').strip()
            ))
    return type_info_t(name, description, fields)

# accepts type name, description and HTML node 'ul' (list) element of subtypes
def parse_oneof_type(name: str, description: str, fieldtable) -> oneof_info_t:
    alts: list[str] = []
    # Собираем все альтернативы из списка
    for row in fieldtable.find_all('li'):
        alts.append(row.find('a').text.strip())

    discriminator_name = None
    alternatives: list[oneof_alternative_t] = []

    for alt in alts:
        h4_node = fieldtable.find_next(lambda tag: tag.name == 'h4' and tag.find('a', href=f'#{alt.lower()}'))
        assert h4_node is not None, f"Не найден элемент h4 для {alt}"

        description_node = h4_node.find_next('p')
        assert description_node is not None, f"Описание не найдено для {alt}"
        alt_description = description_node.text.strip()

        table_node = description_node.find_next('table')
        assert table_node is not None, f"Таблица не найдена для {alt}"

        tmptype: type_info_t = parse_compound_type(alt, description, table_node)

        if name not in TYPES_WITHOUT_DISCRIMINATOR:
            if discriminator_name is None:
                discriminator_name = tmptype.fields[0].name
            else:
                assert discriminator_name == tmptype.fields[0].name, f"Неправильное имя дискриминатора для {alt}"
        PARSED_TYPES[alt] = True
        alternatives.append(oneof_alternative_t(alt, tmptype, alt_description, name))

    return oneof_info_t(name, discriminator_name, description, alternatives)

# accepts HTML with telegram api
# returns pair: array of 'type_info_t' and array of 'oneof_info_t'
def parse_types(tgapi_html: str):
    soup = BeautifulSoup(tgapi_html, 'html.parser')
    types = []
    oneofs = []

    for tag in soup.find_all('h4'):
        type_name = tag.text.strip()
        if type_name in PARSED_TYPES:
            continue
        if not type_name:
            continue
        if len(type_name.split()) != 1:
            continue
        if not type_name[0].isupper():
            continue
        description_tag = tag.find_next_sibling('p')
        if not description_tag:
            continue
        IGNORED_TYPES = {
            'InputFile', # TODO generate by hands HERE (include written file)
            'MaybeInaccessibleMessage' # TODO generate by hands HERE (alias to message)
        }
        PARSED_TYPES[type_name] = True
        if type_name in IGNORED_TYPES:
            continue
        description = description_tag.text.strip()

        if 'Currently holds no information.' in description:
            types.append(type_info_t(type_name, description, []))

        table_or_list = description_tag.find_next()

        while table_or_list and not (table_or_list.name == "table" and "table" in table_or_list.get("class", [])) and table_or_list.name != "ul":
            table_or_list = table_or_list.find_next()
        if table_or_list.name == "table":
            types.append(parse_compound_type(type_name, description, table_or_list))
        elif table_or_list.name == "ul":
            oneofs.append(parse_oneof_type(type_name, description, table_or_list))
        else:
            print(f'SKIPPED: {type_name}')
            continue

    return types, oneofs

######################### GENERATION ########################

def is_boxed_type(tgtype: str) -> bool:
    if tgtype.startswith('arrayof'):
        return False
    return tgtype not in G_TYPE_MAPPING.values()

# TODO check array of arrays
def get_compound_type(tgtype: str):
    x = map_tgtype_to_cpptype(tgtype)
    if x in G_TYPE_MAPPING:
        return None
    return x

def generate_halfoneof_api_struct(t: type_info_t) -> str:
    assert t.is_merged_optional_fields

    # at this point required types must be included

    s = f'struct {t.name} {{\n'

    # mandatory fields

    for f in t.fields:
        if not f.is_optional:
            if is_boxed_type(f.cpptype):
                s += f'  box<{f.cpptype}> {f.name};'
            else:
                s += f'  {f.cpptype} {f.name};'

    # generate field-structs

    for f in t.fields:
        if f.is_optional:
            s += f'  struct {f.name} {{ {f.cppname} value; }};\n'
    
    # data

    s += f'  oneof<{", ".join(e.name for e in t.fields)}> data;\n'

    # enum struct type_e

    s += 'enum struct type_E {\n'
    for f in t.fields:
        if f.is_optional:
            s += f'k_{f.name},\n'
    s += 'nothing,\n};\n'

    # type_e type() const

    s += 'type_e type() const { return static_cast<type_e>(data.index()); }'

    # get ifs

    for f in t.fields:
        if f.is_optional:
            s += f'{f.cpptype}* get_{f.name}() noexcept {{ auto* p = data.get_if<{f.name}>(); return p ? std::addressof(p->value) : nullptr; }}\n'
            s += f'const {f.cpptype}* get_{f.name}() const noexcept {{ auto* p = data.get_if<{f.name}>(); return p ? std::addressof(p->value) : nullptr; }}\n'
    
    # static constexpr decltype(auto) discriminate_field(std::string_view val, auto&& visitor)

    s += 'static constexpr decltype(auto) discriminate_field(std::string_view val, auto&& visitor) {\n'
    for f in t.fields:
        if f.is_optional:
            s += f'  if (val == "{f.cpptype}") return visitor.template operator()<{f.cpptype}>();'
    s += '  return visitor.template operator()<void>();'
    s += '}\n\n'
    s += '};\n' # end struct
    return s

def generate_api_struct(t: type_info_t) -> str:
    if t.is_merged_optional_fields:
        return generate_halfoneof_api_struct(t)

    s = ""
    s += f'/*{t.description}*/\nstruct {t.name} {{\n'

    # mandatory fields first

    for field in t.fields:
        tp = f'  box<{field.cpptype}>' if is_boxed_type(field.cpptype) else field.cpptype
        if not field.is_optional:
            s += f'  {tp} {field.name};\n'

    # optional fields

    for field in t.fields:
        tp = f'  box<{field.cpptype}>' if is_boxed_type(field.cpptype) else field.cpptype
        if field.is_optional:
            s += f'  {tp} {field.name};\n'

    # consteval static bool is_optional_field(std::string_view name)

    s += 'consteval static bool is_optional_field(std::string_view name) {\n'
    s += '  return utils::string_switch<std::optional<bool>>(name)\n'
    for field in t.fields:
        s += f'    .case_("{field.name}", {"true" if field.is_optional else "false"})\n'
    s += '    .or_default(std::nullopt).value();\n'
    s += '}\n\n'
    s += '};\n' # struct end
    
    return s

def cut_oneofname(name: str, ownername: str) -> str:
    assert name.startswith(ownername)
    return name[len(ownername):].lower()

def generate_api_struct_oneof(t: oneof_info_t) -> str:
    if t.name in TYPES_WITHOUT_DISCRIMINATOR:
        # just generate oneof
        return f'/*{t.description}*/\nusing {t.name} = oneof<{", ".join(e.name for e in t.alternatives)}> data;\n'
    s = f'/*{t.description}*/\nstruct {t.name} {{\n'

    # data

    s += f'  oneof<{", ".join(e.name for e in t.alternatives)}> data;\n'

    # static constexpr std::string_view discriminator

    s += f'  static constexpr std::string_view discriminator = {t.discriminator_name};\n'

    # enum struct type_e

    s += '  enum struct type_e {'
    for alt in t.alternatives:
        s += f'k_{cut_oneofname(alt.name, t.name)}, '
    s += 'nothing, }\n'

    # static constexpr size_t variant_size = size_t(type_e::nothing);

    s += 'static constexpr size_t variant_size = size_t(type_e::nothing);'
    
    # type_e type() const;

    s += 'type_e type() const { return static_cast<type_e>(data.index()); }\n'

    # get_ifs

    for alt in t.alternatives:
        s += f'{alt.tgtype}* get_{cut_oneofname(alt.name, t.name)}() noexcept {{ return data.get_if<{alt.tgtype}>(); }}\n'
        s += f'const {alt.tgtype}* get_{cut_oneofname(alt.name, t.name)}() const noexcept {{ return data.get_if<{alt.tgtype}>(); }}\n'

    # static constexpr type_e discriminate(std::string_view val)
    
    s += 'static constexpr type_e discriminate(std::string_view val) {\n'
    s += '  return utils::string_switch<type_e>(val)\n'
    for alt in t.alternatives:
        s += f'    .case_("{alt.name}", type_e::k_{cut_oneofname(alt.name, t.name)})\n)'
    s += '    .or_default(type_e::nothing);\n}\n\n'

    # static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor)

    s += 'static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {\n'
    for alt in t.alternatives:
        s += f'    if (val == "{alt.name}") return visitor.template operator()<{alt.tgtype}>()'
    s += '     return visitor.template operator()<void>();\n'
    s += '}\n\n'
    s += '};\n' # end struct

    return s

# 'required_includes' must be list of pathes in quotes or <>
def generate_into_file(generated_struct: str, filepath: str, required_includes: list[str]):
    with open(filepath, 'w', encoding='utf-8') as out:
        print(f'#pragma once\n', file=out)
        for inc : required_includes:
            print(f'#include {inc}', file=out)
        print('namespace tgbm::api {\n', file=out)
        print(generated_struct, file=out)
        print('\n} // namespace tgbm::api\n', file=out)

def collect_required_includes(t) -> list[str]:
    incs = ['"tgbm/api/common.hpp"', '"tgbm/api/types/fwd.hpp"']
    if not isinstance(t, type_info_t) or not t.is_merged_optional_fields:
        return incs
    for f in t.fields:
        # relative to current dir
        incs.append(f'"{f.cpptype}.hpp"')
    return list(set(incs))

# accepts array of oneofs or simple types
def generate_all_types(types, outdir: str):
    # fill directory with generated files
    for t in types:
        if isinstance(t, type_info_t):
            generate_into_file(generate_api_struct(t), f'{outdir}/{t.name}.hpp', collect_required_includes(t))
        elif isinstance(t, oneof_info_t):
            generate_into_file(generate_api_struct_oneof(t), f'{outdir}/{t.name}.hpp', collect_required_includes(t))
        else:
            assert false
    # all types fwd file
    with open(f'{outdir}/fwd.hpp', 'w', encoding='utf-8') as out:
        print('#pragma once \n\nnamespace tgbm::api {', file=out)
        for t in types:
            if isinstance(t, type_info_t):
                print(f'struct {t.name}', file=out)
        print('} // namespace tgbm::api')

class CustomEncoder(json.JSONEncoder):
    def default(self, obj):
        if hasattr(obj, "to_dict"):
            return obj.to_dict()
        return super().default(obj)

def main():
    #with open('types_tg.json', 'w', encoding='utf-8') as f:
    #    json.dump(ts, f, ensure_ascii=False, indent=2, cls=CustomEncoder)
    #with open('oneofs_tg.json', 'w', encoding='utf-8') as f:
    #    json.dump(ofs, f, ensure_ascii=False, indent=2, cls=CustomEncoder)
    parser = argparse.ArgumentParser()
    parser.add_argument("--outdir", type=str, required=True, help="output dir for types")

    args = parser.parse_args()
    
    ts, ofs = parse_types(load_api_html())
    generate_all_types(ts, args.outdir)
    generate_all_types(ofc, args.outdir)

if __name__ == '__main__':
    main()
