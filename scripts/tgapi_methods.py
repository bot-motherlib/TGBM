from bs4 import BeautifulSoup
import requests
import sys
import typing
import argparse
import os

# loads TG api (html)
def load_api_html():
    url = "https://core.telegram.org/bots/api"
    response = requests.get(url)
    if response.status_code != 200:
        print("[ERROR]: generating methods, cannot load TG API", response.status_code)
        return []
    return response.text

G_REPLY_MARKUP_T = 'oneof<std::monostate, InlineKeyboardMarkup, ReplyKeyboardMarkup, ReplyKeyboardRemove, ForceReply>'

G_FILEORSTR = "file_or_str"
G_TYPE_MAPPING = {
    # TODO rename integer.hpp (first letter)
    "integer": "Integer", # TODO renmae into api::integer
    "string": "String", # TODO rename into api::string
    "integerorstring": "int_or_str",
    "boolean": "bool",
    "float": "double",
    "inputfileorstring": G_FILEORSTR,
    "inputfile": "InputFile",
    "inlinekeyboardmarkuporreplykeyboardmarkuporreplykeyboardremoveorforcereply": "reply_markup_t"
}

def is_boxed_type(tgtype: str) -> bool:
    if tgtype.startswith('arrayof'):
        return False
    return tgtype not in G_TYPE_MAPPING.values()

# maps raw parsed tg type to cpp type   
def map_tgtype_to_cpptype(tgtype):
    typeunified = tgtype.strip().lower()
    typeunified = typeunified.replace(' ', '')
    if typeunified in G_TYPE_MAPPING:
        return G_TYPE_MAPPING[typeunified]
    if typeunified.startswith('arrayof'):
        tgtype = tgtype.replace(' ', '')
        return 'arrayof<'+ map_tgtype_to_cpptype(tgtype[len('arrayof'):]) + '>'
    return tgtype # unmodified, just complex type

# takes compound part from cpptype (array or not)
# returns none if not compound
def get_compound_type(cpptype: str):
    if cpptype.startswith('arrayof<'):
        cpptype = cpptype[len('arrayof<'):-len('>')]
        assert not cpptype.startswith('arrayof') # TG never returns array of array
        if cpptype in G_TYPE_MAPPING.values():
            return None
        else:
            return cpptype # array of compound type
    if cpptype in G_TYPE_MAPPING.values():
        return None
    return cpptype # compound type

class param_t:
    def __init__(self, name: str, param_type: str, is_optional: bool, description: str):
        self.name = name
        self.param_type = param_type
        self.is_optional = is_optional
        self.description = description

class method_info_t:
    def __init__(self, name, description):
        self.name = name
        self.parameters: List[Parameter] = []
        self.description = description

def is_file_arg(m: param_t):
    return m.param_type == G_FILEORSTR or m.param_type == "InputFile"

# accepts HTML with telegram api
# returns array of 'method_info'
def parse_methods(tgapi_html):
    soup = BeautifulSoup(tgapi_html, 'html.parser')

    api_methods = []

    for method_header in soup.find_all('h4'):
        method_name = method_header.text.strip()
        if not method_name[0].islower():
            continue # type, not method
        description_tag = method_header.find_next_sibling('p')
        if not description_tag:
            continue
        description = description_tag.text.strip()
        mi = method_info_t(method_name, description)
        if "Requires no parameters" in description:
            api_methods.append(mi)
            continue
        param_table = method_header.find_next('table')
        if not param_table:
            continue
        for row in param_table.find_all('tr')[1:]:
            cells = row.find_all('td')
            if len(cells) != 4:
                continue # some incorrect table
            mi.parameters.append(param_t(
                name=cells[0].text.strip(),
                param_type=map_tgtype_to_cpptype(cells[1].text.strip()),
                is_optional= 'Yes' not in cells[2].text.strip(),
                description=cells[3].text.strip()))
        if len(mi.parameters) == 0:
            continue # skip not valid params, just something h4
        api_methods.append(mi)
    return api_methods

# getUpdates -> get_updates
def to_flat_naming(name: str):
    r = ""
    assert len(name) != 0 and name[0].islower()
    for c in name:
        if c.isupper():
            r += '_'
            r += c.lower()
        else:
            r += c
    return r

# returns collected 'struct *something*;' strings
def collect_required_forwards(method_desc: method_info_t) -> list[str]:
    fwds = []
    for param in method_desc.parameters:
        compound = get_compound_type(param.param_type)
        if compound:
            fwds.append(f'struct {compound};') # compound type
    return list(set(fwds))

# returns string with generated C++ code of api struct
def generate_api_struct(method_desc: method_info_t):
    s = ""
    s += f"/* {method_desc.description} */\n"

    s += f"struct {to_flat_naming(method_desc.name) + '_r'} {{\n"
    for param in method_desc.parameters:
        if not param.is_optional:
            # /* {param['description']} */\n
            if is_boxed_type(param.param_type):
                s += f'  box<{param.param_type}> {param.name};\n'
            else:
                s += f"  {param.param_type} {param.name};\n"
    # TODO sort by sizeof in groups...
    for param in method_desc.parameters:
        if param.is_optional:
            # /* {param['description']} */\n
            if is_boxed_type(param.param_type):
                s += f'  box<{param.param_type}> {param.name};\n'
            else:
                s += f"  optional<{param.param_type}> {param.name};\n"
    s += '\n'

    # static constexpr file_info_e file_info

    s += '  static constexpr file_info_e file_info = file_info_e::'
    if any(e.param_type == 'InputFile' for e in method_desc.parameters):
        file_kind = 'yes'
    elif any(e.param_type == G_FILEORSTR for e in method_desc.parameters):
        file_kind = 'maybe'
    else:
        file_kind = 'no'
    s += f'{file_kind};\n'

    # static constexpr std::string_view api_method_name

    s += f'  static constexpr std::string_view api_method_name = "{method_desc.name}";\n'

    # static constexpr http_method_e http_method

    s += f'  static constexpr http_method_e http_method = http_method_e::{"POST" if len(method_desc.parameters) != 0 else "GET"};\n'

    if len(method_desc.parameters) == 0:
        s += '};\n'
        return s # GET methods without args

    # void fill_nonfile_args(auto& body)

    s += '\n  void fill_nonfile_args(auto& body) {\n'

    for param in method_desc.parameters:
        if param.param_type == G_FILEORSTR:
            # assume is str, because file must be filled in other fn
            if param.is_optional:
                s += f'    if ({param.name})\n'
                s += f'      body.arg("{param.name}", *(*{param.name}).get_str());\n'
            else:
                s += f'    body.arg("{param.name}", *{param.name}.get_str());\n'
        elif param.param_type == 'InputFile':
            continue # must be filled in 'fill_file_args'
        else:
            if param.is_optional:
                s += f'    if ({param.name})\n'
                s += f'      body.arg("{param.name}", *{param.name});\n'
            else:
                s += f'    body.arg("{param.name}", {param.name});\n'
    s += '  }\n'

    if file_kind == 'no':
        s += '};\n'
        return s # simple methods without files

    # bool has_file_args()

    s += '\n  [[nodiscard]] bool has_file_args() const noexcept {\n'
    required_return_false = True
    for param in method_desc.parameters:
        if param.param_type == 'InputFile':
            if not param.is_optional:
                required_return_false = False
                s += '    return true;\n'
            else:
                s += f'    if ({param.name})\n'
                s += '       return true;\n' 
        elif param.param_type == G_FILEORSTR:
            s += f'    if ({param.name}.is_file())\n'
            s += '       return true;\n'
    if required_return_false:
        s += '    return false;\n'
    s += '  }\n'

    # void fill_file_args(application_multipart_form_data& body)

    s += '\n  void fill_file_args(application_multipart_form_data& body) {\n'
    for param in method_desc.parameters:
        # using overload for InputFile in multipart body
        if param.param_type == 'InputFile':
            if param.is_optional:
                s += f'    if ({param.name})\n'
                s += f'      body.arg("{param.name}", *{param.name});\n'
            else:
                s += f'    body.arg("{param.name}", {param.name});\n'
        elif param.param_type == G_FILEORSTR:
            if param.is_optional:
                s += f'    if (${param.name}) if (InputFile* f = std::get_if<InputFile>(&{param.name}))\n'
                s += f'      body.arg("{param.name}", *f);\n'
            else:
                s += f'    if (InputFile* f = std::get_if<InputFile>(&{param.name}))\n'
                s += f'      body.arg("{param.name}", *f);\n'
    s += '  }\n'

    s += '};\n'
    return s

# overwrites existing file
def generate_into_file(method_desc: method_info_t, filepath: str):
    with open(filepath, 'w', encoding='utf-8') as out:
        print(f'#pragma once\n\n', file=out)
        print('#include "tgbm/api/common.hpp"\n', file=out)
        print('namespace tgbm::api {\n', file=out)
        for fwd in collect_required_forwards(method_desc):
            print(f'{fwd}', file=out)
        print(f'{generate_api_struct(method_desc)}', file=out)
        print('\n} // namespace tgbm::api', file=out)

def generate_all_methods(methods: list[method_info_t], outdir: str):
    os.makedirs(outdir, exist_ok=True)
    # TODO: also api/types.hpp (all file)
    for m in methods:
        generate_into_file(m, f'{outdir}/{m.name}.hpp')
        # TODO clang-format
    with open(f'{outdir}/methods.hpp', 'w', encoding='utf-8') as out:
        print('#pragma once \n\n', file=out)
        for m in methods:
            print(f'#include "{outdir}/{m.name}.hpp"\n', file=out)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--outdir", type=str, required=True, help="output dir for methods")

    args = parser.parse_args()
    
    generate_all_methods(parse_methods(load_api_html()), args.outdir)

if __name__ == '__main__':
    main()
# TODO handle reply makrup and variants (startswith oneof) in generating request serializing
# TODO specialization (by hands) parsing return type
# TODO write into file (in selected dir + clang-format it)
# TODO write concept bot_api_request
# TODO write task<result_of<Request>> send_request(http_client&, const bot_api_request auto&, duration_t timeout)
# (and not coroutine internaly, return coroutine after creating body)
# TODO: same with ignoring return value
# TODO: <api_request>_builder (function which accepts required arguments or just... Everytime multipart body if file possible!!!
# for removing problem
