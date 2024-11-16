from bs4 import BeautifulSoup

import typing
import argparse
import os
import re
import json

def load_file(path: str) -> str:
    with open(path, 'r', encoding='utf-8') as file:
        content = file.read()
    return content

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

# original - not modified type
def unify(somestr: str) -> str:
    return somestr.strip().lower().replace(' ', '').replace('\n', '')
def map_tgtype_to_cpptype(tgtype: str):
    typeunified = unify(tgtype)
    if typeunified in G_TYPE_MAPPING:
        return G_TYPE_MAPPING[typeunified]
    if typeunified.startswith('arrayof'):
        # Array of X / Array of Array of X
        s = tgtype.strip().replace('\n', ' ').split()[-1]
        su = s.replace(' ', '').lower()
        if su in G_TYPE_MAPPING:
            return f'arrayof<{G_TYPE_MAPPING[su]}>'
        else:
            # not remove UpperLatters
            return f'arrayof<{s}>'
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
        mand = [*self.mandatory_fields()]
        opt_bools = [f for f in self.optional_fields() if (f.cpptype == 'bool' or f.cpptype == 'True')]
        opt_not_bools = [f for f in self.optional_fields() if not (f.cpptype == 'bool' or f.cpptype == 'True')]
        self.fields = [*mand, *opt_not_bools, *opt_bools]

    def to_dict(self):
        return {
            "name": self.name,
            "description": self.description,
            "is_merged_optional_fields": self.is_merged_optional_fields,
            "fields": self.fields,
        }
    
    def optional_fields(self):
        return [field for field in self.fields if field.is_optional]

    def mandatory_fields(self):
        return [field for field in self.fields if not field.is_optional]

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

    for row in fieldtable.find_all('li'):
        alts.append(row.find('a').text.strip())

    discriminator_name = None
    alternatives: list[oneof_alternative_t] = []

    for alt in alts:
        h4_node = fieldtable.find_next(lambda tag: tag.name == 'h4' and tag.find('a', href=f'#{alt.lower()}'))
        assert h4_node is not None

        description_node = h4_node.find_next('p')
        assert description_node is not None
        alt_description = description_node.text.strip()

        table_node = description_node.find_next('table')
        assert table_node is not None

        tmptype: type_info_t = parse_compound_type(alt, description, table_node)

        if name not in TYPES_WITHOUT_DISCRIMINATOR:
            if discriminator_name is None:
                discriminator_name = tmptype.fields[0].name
            else:
                assert discriminator_name == tmptype.fields[0].name
        PARSED_TYPES[alt] = True
        alternatives.append(oneof_alternative_t(alt, tmptype, alt_description, name))

    return oneof_info_t(name, discriminator_name, description, alternatives)

IGNORED_TYPES = {
    'InputFile',
    'MaybeInaccessibleMessage'
}

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

def get_compound_type(tgtype: str):
    x = map_tgtype_to_cpptype(tgtype)
    while x.startswith('arrayof<'):
        x = x[len('arrayof<'):-1]
    if x in G_TYPE_MAPPING.values():
        return None
    return x

def is_boxed_type(tgtype: str) -> bool:
    tmp = unify(tgtype)
    if tmp.startswith('arrayof'):
        return False
    return get_compound_type(tgtype) is not None

def generate_halfoneof_api_struct(t: type_info_t) -> str:
    assert t.is_merged_optional_fields

    # at this point required types must be included

    s = f'struct {t.name} {{\n'

    # mandatory fields

    for f in t.mandatory_fields():
        s += f'/* {f.description} */\n'
        if is_boxed_type(f.cpptype):
            s += f'  box<{f.cpptype}> {f.name};'
        else:
            s += f'  {f.cpptype} {f.name};'

    # generate field-structs

    for f in t.optional_fields():
        s += f'  struct {f.name} {{ {f.cpptype} value; }};\n'

    # data

    s += f'  oneof<{", ".join(f.name for f in t.optional_fields())}> data;\n'

    # enum struct type_e

    s += '  enum struct type_e {\n'
    for f in t.optional_fields():
        s += f'/* {f.description} */\n'
        s += f'    k_{f.name},\n'
    s += '    nothing,\n  };\n'

    # static constexpr size_t variant_size

    s += '  static constexpr size_t variant_size = size_t(type_e::nothing);\n'

    # type_e type() const

    s += '  type_e type() const { return static_cast<type_e>(data.index()); }\n'

    # get ifs

    for f in t.optional_fields():
        s += f'  {f.cpptype}* get_{f.name}() noexcept {{ auto* p = data.get_if<{f.name}>(); return p ? &p->value : nullptr; }}\n'
        s += f'  const {f.cpptype}* get_{f.name}() const noexcept {{ auto* p = data.get_if<{f.name}>(); return p ? &p->value : nullptr; }}\n'
    
    # static constexpr decltype(auto) discriminate_field(std::string_view val, auto&& visitor)

    s += '  static constexpr decltype(auto) discriminate_field(std::string_view val, auto&& visitor) {\n'
    for f in t.optional_fields():
        s += f'    if (val == "{f.name}") return visitor.template operator()<{f.name}>();'
    s += '    return visitor.template operator()<void>();'
    s += '  }\n\n'

    # std::string_view discriminator_now() const

    s += f'std::string_view discriminator_now() const noexcept {{\n'
    s += f'using enum {t.name}::type_e;'
    s += 'switch(type()) {\n'
    for f in t.optional_fields():
        s += f' case k_{f.name}: return "{f.name}";\n'
    s += 'case nothing: return "";\n'
    s += 'default: unreachable();\n'
    s += '}\n}\n'

    s += '};\n' # end struct

    return s

def generate_api_struct(t: type_info_t) -> str:
    if t.is_merged_optional_fields:
        return generate_halfoneof_api_struct(t)

    s = ""
    s += f'/*{t.description}*/\nstruct {t.name} {{\n'

    # mandatory fields first

    for field in t.mandatory_fields():
        s += f'/* {field.description} */\n'
        tp = f'  box<{field.cpptype}>' if is_boxed_type(field.cpptype) else field.cpptype
        s += f'  {tp} {field.name};\n'

    # optional fields

    for field in t.optional_fields():
        s += f'/* {field.description} */\n'
        if is_boxed_type(field.cpptype):
            s += f'  box<{field.cpptype}> {field.name};\n'
        else:
            s += f'  optional<{field.cpptype}> {field.name};\n'

    # consteval static bool is_mandatory_field(std::string_view name)

    s += '\n  consteval static bool is_mandatory_field(std::string_view name) {\n'
    s += '    return string_switch<bool>(name)\n'
    for f in t.mandatory_fields():
        s += f'    .case_("{f.name}", true)\n'
    s += '    .or_default(false);\n'
    s += '  }\n\n'
    s += '};\n' # struct end

    return s

def cut_oneofname(name: str, ownername: str) -> str:
    assert name.startswith(ownername)
    return name[len(ownername):].lower()

def generate_api_struct_oneof(t: oneof_info_t) -> str:
    if t.name in TYPES_WITHOUT_DISCRIMINATOR:
        # just generate oneof
        return f'/*{t.description}*/\nusing {t.name} = oneof<{", ".join(e.name for e in t.alternatives)}>;\n'
    s = f'/*{t.description}*/\nstruct {t.name} {{\n'

    # data

    s += f'  oneof<{", ".join(e.name for e in t.alternatives)}> data;\n'

    # static constexpr std::string_view discriminator

    s += f'  static constexpr std::string_view discriminator = "{t.discriminator_name}";\n'

    # enum struct type_e

    s += '  enum struct type_e {'
    for alt in t.alternatives:
        s += f'    k_{cut_oneofname(alt.name, t.name)}, '
    s += '    nothing, };\n\n'

    # static constexpr size_t variant_size = size_t(type_e::nothing);

    s += '  static constexpr size_t variant_size = size_t(type_e::nothing);\n'
    
    # type_e type() const;

    s += '  type_e type() const { return static_cast<type_e>(data.index()); }\n'

    # get_ifs

    for alt in t.alternatives:
        s += f'  {alt.tgtype.name}* get_{cut_oneofname(alt.name, t.name)}() noexcept {{ return data.get_if<{alt.tgtype.name}>(); }}\n'
        s += f'  const {alt.tgtype.name}* get_{cut_oneofname(alt.name, t.name)}() const noexcept {{ return data.get_if<{alt.tgtype.name}>(); }}\n'

    # static constexpr type_e discriminate(std::string_view val)
    
    s += '  static constexpr type_e discriminate(std::string_view val) {\n'
    s += '    return string_switch<type_e>(val)\n'
    for alt in t.alternatives:
        s += f'    .case_("{alt.discriminator_value}", type_e::k_{cut_oneofname(alt.name, t.name)})\n'
    s += '    .or_default(type_e::nothing);\n}\n\n'

    # static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor)

    s += '  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {\n'
    for alt in t.alternatives:
        s += f'    if (val == "{alt.discriminator_value}") return visitor.template operator()<{alt.name}>();'
    s += '    return visitor.template operator()<void>();\n'
    s += '  }\n\n'

    # std::string_view discriminator_now() const

    s += f'std::string_view discriminator_now() const noexcept {{\n'
    s += f'  using enum {t.name}::type_e;'
    s += '  switch(type()) {\n'
    for alt in t.alternatives:
        s += f' case k_{cut_oneofname(alt.name, t.name)}: return "{alt.discriminator_value}";\n'
    s += 'case nothing: return "";\n'
    s += 'default: unreachable();\n'
    s += '  }\n}\n'

    s += '};\n' # end struct

    return s

# 'required_includes' must be list of pathes in quotes or <>
def generate_into_file(generated_struct: str, filepath: str, required_includes: list[str]):
    with open(filepath, 'w', encoding='utf-8') as out:
        print(f'#pragma once\n', file=out)
        for inc in required_includes:
            print(f'#include {inc}', file=out)
        print('\nnamespace tgbm::api {\n', file=out)
        print(generated_struct, file=out)
        print('\n} // namespace tgbm::api\n', file=out)

def collect_required_includes(t) -> list[str]:
    # relative dir /all_fwd.hpp
    incs = ['"all_fwd.hpp"']
    if isinstance(t, oneof_info_t):
        for alt in t.alternatives:
            incs.append(f'"{alt.tgtype.name}.hpp"')
        return incs
    if not isinstance(t, type_info_t) or not t.is_merged_optional_fields:
        return incs
    for f in t.fields:
        # relative to current dir
        t = get_compound_type(f.cpptype)
        if t:
            incs.append(f'"{t}.hpp"')
    return sorted(list(set(incs)))

# accepts array of oneofs or simple types
def generate_all_types(types: list[type_info_t], outdir: str):
    # fill directory with generated files
    for t in types:
        print(f'[TGBM] generating "{outdir}/{t.name}.hpp"')
        generate_into_file(generate_api_struct(t), f'{outdir}/{t.name}.hpp', collect_required_includes(t))

    # IGNORED_TYPES

    for t in IGNORED_TYPES:
        assert t in PARSED_TYPES
    # InputFile
    with open(f'{outdir}/InputFile.hpp', 'w', encoding='utf-8') as out:
        print('#include "tgbm/api/input_file.hpp"', file=out)

    # MaybeInaccessibleMessage

    with open(f'{outdir}/MaybeInaccessibleMessage.hpp', 'w', encoding='utf-8') as out:
        print('#pragma once\n', file=out)
        print('#include "Message.hpp"\n\nnamespace tgbm::api {\n\nusing MaybeInaccessibleMessage = Message;\n\n}', file=out)

def generate_all_fwd_and_all_types_hdrs(outdir: str):
    # all_fwd.hpp

    print(f'[TGBM] generating "{outdir}/all_fwd.hpp"')
    with open(f'{outdir}/all_fwd.hpp', 'w', encoding='utf-8') as out:
        print('#pragma once \n\n#include "tgbm/api/common.hpp"\n\nnamespace tgbm::api {\n', file=out)
        for t in PARSED_TYPES.keys():
            if t != 'MaybeInaccessibleMessage' and t not in TYPES_WITHOUT_DISCRIMINATOR:
                print(f'struct {t};', file=out)
        print('using MaybeInaccessibleMessage = Message;', file=out)
        # InputMessageContent
        print('using InputMessageContent = oneof<InputTextMessageContent, InputLocationMessageContent, InputVenueMessageContent, InputContactMessageContent, InputInvoiceMessageContent>;', file=out)
        print('\n} // namespace tgbm::api', file=out)

    # all.hpp

    with open(f'{outdir}/all.hpp', 'w', encoding='utf-8') as out:
        print('#pragma once \n\n', file=out)
        for t in PARSED_TYPES.keys():
            print(f'#include "{t}.hpp"', file=out)

def generate_all_oneofs(types: list[oneof_info_t], outdir: str):
    # fill directory with generated files
    for t in types:
        print(f'[TGBM] generating "{outdir}/{t.name}.hpp"')
        generate_into_file(generate_api_struct_oneof(t), f'{outdir}/{t.name}.hpp', collect_required_includes(t))
    subtypes : list[type_info_t]= []
    for t in types:
        for alt in t.alternatives:
            subtypes.append(alt.tgtype)
    subtypes = list(set(subtypes))
    generate_all_types(subtypes, outdir)

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
    parser.add_argument("--outdir", type=str, required=True, help="output dir for types(without quotes)")
    parser.add_argument("--apifile", type=str, required=True, help="file with loaded TG api (HTML) (without quotes)")

    args = parser.parse_args()

    ts, ofs = parse_types(load_file(args.apifile))

    print(f'[TGBM] creating directory: {args.outdir}\n')
    os.makedirs(args.outdir, exist_ok=True)

    generate_all_types(ts, args.outdir)
    generate_all_oneofs(ofs, args.outdir)
    generate_all_fwd_and_all_types_hdrs(args.outdir)

if __name__ == '__main__':
    main()
