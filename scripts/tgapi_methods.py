from bs4 import BeautifulSoup

import typing
import argparse
import os

def load_file(path: str) -> str:
    with open(path, 'r', encoding='utf-8') as file:
        content = file.read()
    return content

MSG = 'Message'
TRUE = 'bool' # True mapped to bool
MSGID_ARRAY = 'arrayof<MessageId>'
STRING = 'String'
CHATINVLINK = 'ChatInviteLink'
CHATMEMBER = 'ChatMember'
MSGORTRUE = f'oneof<{TRUE}, {MSG}>'
FILE = 'File'

G_OPERATION_TO_RESULT = {
    'getUpdates': 'arrayof<Update>',
    'setWebhook': TRUE,
    'deleteWebhook': TRUE,
    'getWebhookInfo': 'WebhookInfo',
    'getMe': 'User',
    'logOut': TRUE,
    'close': TRUE,
    'sendMessage': MSG,
    'forwardMessage': MSG,
    'forwardMessages': MSGID_ARRAY,
    'copyMessage': 'MessageId',
    'copyMessages': MSGID_ARRAY,
    'sendPhoto': MSG,
    'sendAudio': MSG,
    'sendDocument': MSG,
    'sendVideo': MSG,
    'sendAnimation': MSG,
    'sendVoice': MSG,
    'sendVideoNote': MSG,
    'sendPaidMedia': MSG,
    'sendMediaGroup': f'arrayof<{MSG}>',
    'sendLocation': MSG,
    'sendVenue': MSG,
    'sendContact': MSG,
    'sendPoll': MSG,
    'sendDice': MSG,
    'sendChatAction': TRUE,
    'setMessageReaction': TRUE,
    'getUserProfilePhotos': 'UserProfilePhotos',
    'getFile': FILE,
    'banChatMember': TRUE,
    'unbanChatMember': TRUE,
    'restrictChatMember': TRUE,
    'promoteChatMember': TRUE,
    'setChatAdministratorCustomTitle': TRUE,
    'banChatSenderChat': TRUE,
    'unbanChatSenderChat': TRUE,
    'setChatPermissions': TRUE,
    'exportChatInviteLink': STRING,
    'createChatInviteLink': CHATINVLINK,
    'editChatInviteLink': CHATINVLINK,
    'createChatSubscriptionInviteLink': CHATINVLINK,
    'editChatSubscriptionInviteLink': CHATINVLINK,
    'revokeChatInviteLink': CHATINVLINK,
    'approveChatJoinRequest': TRUE,
    'declineChatJoinRequest': TRUE,
    'setChatPhoto': TRUE,
    'deleteChatPhoto': TRUE,
    'setChatTitle': TRUE,
    'setChatDescription': TRUE,
    'pinChatMessage': TRUE,
    'unpinChatMessage': TRUE,
    'unpinAllChatMessages': TRUE,
    'leaveChat': TRUE,
    'getChat': 'ChatFullInfo',
    'getChatAdministrators': f'arrayof<{CHATMEMBER}>',
    'getChatMemberCount': 'Integer',
    'getChatMember': CHATMEMBER,
    'setChatStickerSet': TRUE,
    'deleteChatStickerSet': TRUE,
    'getForumTopicIconStickers': 'arrayof<Sticker>',
    'createForumTopic': 'ForumTopic',
    'editForumTopic': TRUE,
    'closeForumTopic': TRUE,
    'reopenForumTopic': TRUE,
    'deleteForumTopic': TRUE,
    'unpinAllForumTopicMessages': TRUE,
    'editGeneralForumTopic': TRUE,
    'closeGeneralForumTopic': TRUE,
    'reopenGeneralForumTopic': TRUE,
    'hideGeneralForumTopic': TRUE,
    'unhideGeneralForumTopic': TRUE,
    'unpinAllGeneralForumTopicMessages': TRUE,
    'answerCallbackQuery': TRUE,
    'getUserChatBoosts': 'UserChatBoosts',
    'getBusinessConnection': 'BusinessConnection',
    'setMyCommands': TRUE,
    'deleteMyCommands': TRUE,
    'getMyCommands': 'arrayof<BotCommand>',
    'setMyName': TRUE,
    'getMyName': 'BotName',
    'setMyDescription': TRUE,
    'getMyDescription': 'BotDescription',
    'setMyShortDescription': TRUE,
    'getMyShortDescription': 'BotShortDescription',
    'setChatMenuButton': TRUE,
    'getChatMenuButton': 'MenuButton',
    'setMyDefaultAdministratorRights': TRUE,
    'getMyDefaultAdministratorRights': 'ChatAdministratorRights',
    'editMessageText': MSGORTRUE,
    'editMessageCaption': MSGORTRUE,
    'editMessageMedia': MSGORTRUE,
    'editMessageLiveLocation': MSGORTRUE,
    'stopMessageLiveLocation': MSGORTRUE,
    'editMessageReplyMarkup': MSGORTRUE,
    'stopPoll': 'Poll',
    'deleteMessage': TRUE,
    'deleteMessages': TRUE,
    'sendSticker': MSG,
    'getStickerSet': 'StickerSet',
    'getCustomEmojiStickers': 'arrayof<Sticker>',
    'uploadStickerFile': FILE,
    'createNewStickerSet': TRUE,
    'addStickerToSet': TRUE,
    'setStickerPositionInSet': TRUE,
    'deleteStickerFromSet': TRUE,
    'replaceStickerInSet': TRUE,
    'setStickerEmojiList': TRUE,
    'setStickerKeywords': TRUE,
    'setStickerMaskPosition': TRUE,
    'setStickerSetTitle': TRUE,
    'setStickerSetThumbnail': TRUE,
    'setCustomEmojiStickerSetThumbnail': TRUE,
    'deleteStickerSet': TRUE,
    'answerInlineQuery': TRUE,
    'answerWebAppQuery': 'SentWebAppMessage',
    'sendInvoice': MSG,
    'createInvoiceLink': STRING,
    'answerShippingQuery': TRUE,
    'answerPreCheckoutQuery': TRUE,
    'getStarTransactions': 'StarTransactions',
    'refundStarPayment': TRUE,
    'setPassportDataErrors': TRUE,
    'sendGame': MSG,
    'setGameScore': MSGORTRUE,
    'getGameHighScores': 'arrayof<GameHighScore>'
}

# loads TG api (html)
def load_api_html():
    url = "https://core.telegram.org/bots/api"
    response = requests.get(url)
    if response.status_code != 200:
        print("[ERROR]: generating methods, cannot load TG API", response.status_code)
        return []
    return response.text

G_FILEORSTR = "file_or_str"
G_TYPE_MAPPING = {
    "integer": "Integer",
    "string": "String",
    "integerorstring": "int_or_str",
    "boolean": "bool",
    "float": "Double",
    "inputfileorstring": G_FILEORSTR,
    "inputfile": "InputFile",
    "inlinekeyboardmarkuporreplykeyboardmarkuporreplykeyboardremoveorforcereply": "reply_markup_t"
}

# takes compound part from cpptype (array or not)
# returns none if not compound
def get_compound_type(tgtype: str):
    x = map_tgtype_to_cpptype(tgtype)
    while x.startswith('arrayof<'):
        x = x[len('arrayof<'):-1]
    if x in G_TYPE_MAPPING.values():
        return None
    return x

def unify(somestr: str) -> str:
    return somestr.strip().lower().replace(' ', '').replace('\n', '')

def is_boxed_type(tgtype: str) -> bool:
    tmp = unify(tgtype)
    if tmp.startswith('arrayof'):
        return False
    return get_compound_type(tgtype) is not None

# maps raw parsed tg type to cpp type   
def map_tgtype_to_cpptype(tgtype: str) -> str:
    typeunified = tgtype.strip().lower()
    typeunified = typeunified.replace(' ', '')
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

class param_t:
    def __init__(self, name: str, param_type: str, is_optional: bool, description: str):
        self.name = name
        self.param_type = param_type
        self.is_optional = is_optional
        self.description = description

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

class method_info_t:
    def __init__(self, name, description):
        self.name = name
        self.parameters: List[Parameter] = []
        self.description = description
        self.ret_type = G_OPERATION_TO_RESULT[name]
        assert self.ret_type is not None
    
    def get_cppstruct_name(self) -> str:
        return to_flat_naming(self.name) + '_request'

def is_file_arg(m: param_t):
    return m.param_type == G_FILEORSTR or m.param_type == "InputFile"

def parse_methods(tgapi_html: str) -> list[method_info_t]:
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

# returns string with generated C++ code of api struct
def generate_api_struct(method_desc: method_info_t):
    s = ""
    s += f"struct {method_desc.get_cppstruct_name()} {{\n"
    for param in method_desc.parameters:
        if not param.is_optional:
            s += f'/* {param.description} */\n'
            if is_boxed_type(param.param_type):
                s += f'  box<{param.param_type}> {param.name};\n'
            else:
                s += f"  {param.param_type} {param.name};\n"

    for param in method_desc.parameters:
        if param.is_optional:
            s += f'/* {param.description} */\n'
            if is_boxed_type(param.param_type):
                s += f'  box<{param.param_type}> {param.name};\n'
            else:
                s += f"  optional<{param.param_type}> {param.name};\n"
    s += '\n'

    # using return_type

    typ = G_OPERATION_TO_RESULT[method_desc.name]
    s += f'using return_type = {typ};\n'

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

    # void fill_nonfile_args(auto& body) const

    s += '\n  void fill_nonfile_args(auto& body) const {\n'

    for param in method_desc.parameters:
        if param.param_type == G_FILEORSTR:
            # assume is str, because file must be filled in other fn
            if param.is_optional:
                s += f'    if ({param.name}) if (const auto* str = {param.name}->get_str())'
                s += f'      body.arg("{param.name}", *str);\n'
            else:
                s += f'    if (const auto* str = {param.name}.get_str()) body.arg("{param.name}", *str);\n'
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
                s += f'    if ({param.name}) return true;\n'
        elif param.param_type == G_FILEORSTR:
            if param.is_optional:
                s += f'if ({param.name} && {param.name}->is_file()) return true;\n'
            else:
                s += f'if ({param.name}.is_file()) return true;\n'
    if required_return_false:
        s += '    return false;\n'
    s += '  }\n'

    # void fill_file_args(application_multipart_form_data& body) const

    s += '\n  void fill_file_args(application_multipart_form_data& body) const {\n'
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
                s += f'    if ({param.name}) if (const InputFile* f = {param.name}->get_file())\n'
                s += f'      body.arg("{param.name}", *f);\n'
            else:
                s += f'    if (const InputFile* f = {param.name}.get_file())\n'
                s += f'      body.arg("{param.name}", *f);\n'
    s += '  }\n'

    s += '};\n'
    return s

# returns array of type names
def collect_required_includes(method_desc: method_info_t) -> list[str]:
    ts = []
    for p in method_desc.parameters:
        ct = get_compound_type(p.param_type)
        if ct:
            ts.append(f'{ct}')
    # return type
    typ = G_OPERATION_TO_RESULT[method_desc.name]
    if typ != MSGORTRUE:
        typ = get_compound_type(typ)
        if typ:
            ts.append(typ)
    else:
        ts.append(MSG)

    return list(set(ts))

# overwrites existing file
def generate_into_file(method_desc: method_info_t, filepath: str):
    with open(filepath, 'w', encoding='utf-8') as out:
        print(f'#pragma once\n', file=out)
        print('#include "tgbm/api/common.hpp"', file=out)
        for inc in collect_required_includes(method_desc):
            print(f'#include "tgbm/api/types/{inc}.hpp"', file=out)
        print('\nnamespace tgbm::api {\n', file=out)
        print(f'{generate_api_struct(method_desc)}', file=out)
        print('\n} // namespace tgbm::api', file=out)

def generate_all_methods(methods: list[method_info_t], outdir: str):
    for m in methods:
        print(f'[TGBM] generating "{outdir}/{m.name}.hpp"')
        generate_into_file(m, f'{outdir}/{m.name}.hpp')

    # all_fwd.hpp
    with open(f'{outdir}/all_fwd.hpp', 'w', encoding='utf-8') as out:
        print('#pragma once\n', file=out)
        print('namespace tgbm::api {\n', file=out)
        for m in methods:
            print(f'struct {m.get_cppstruct_name()};', file=out)
        print('\n}', file=out) # namespace tgbm::api

    # all.hpp
    with open(f'{outdir}/all.hpp', 'w', encoding='utf-8') as out:
        print('#pragma once\n', file=out)
        for m in methods:
            print(f'#include "{m.name}.hpp"', file=out)

################################## API generation #################################

# out - file
def generate_api_fwd_header(methods: list[method_info_t], out):
    print(f'#pragma once\n', file=out)
    print('#include <kelcoro/task.hpp>\n\n#include "tgbm/net/http_client.hpp"\n#include "tgbm/api/methods/all.hpp"\n', file=out)
    print('namespace tgbm::api {\n', file=out)
    print('struct telegram {', file=out)
    # fields and ctor
    print('\nhttp_client& client;\nconst_string bottoken;\n', file=out)
    for m in methods:
        apistructarg = f'api::{m.get_cppstruct_name()}, ' if len(m.parameters) != 0 else ""
        print(f'/* {m.description} */\ndd::task<{m.ret_type}> {m.name}({apistructarg} duration_t timeout = duration_t::max());\n', file=out)
    print('\n};\n', file=out) # struct telegram
    print('}\n', file=out) # namespace tgbm::api

# out - file
def generate_api_impl_file(methods: list[method_info_t], out):
    print('#include "tgbm/api/telegram.hpp"\n', file=out)
    print('#include "tgbm/api/types/all.hpp"\n#include "tgbm/api/methods/all.hpp"\n#include "tgbm/api/const_string.hpp"', file=out)
    print('#include "tgbm/api/requests.hpp"\n', file=out)
    print('namespace tgbm::api {\n', file=out)
    for m in methods:
        if len(m.parameters) == 0:
            implstr = f'''
            dd::task<{m.ret_type}> telegram::{m.name}(duration_t timeout) {{
                {m.ret_type}& result = co_await dd::this_coro::return_place;
                reqerr_t err = co_await api::send_request(api::{m.get_cppstruct_name()}{{}}, client, bottoken, result, timeout);
                if (err) [[unlikely]] {{
                    TGBM_LOG_ERROR("{m.name} request ended with error, status: {{}}, description: {{}}", err.status, err.description.str());
                    handle_telegram_http_status(err.status);
                }}
                co_return dd::rvo;
            }}
            '''
            print(implstr, file=out)
        else:
            implstr = f'''
            dd::task<{m.ret_type}> telegram::{m.name}(api::{m.get_cppstruct_name()} request, duration_t timeout) {{
                {m.ret_type}& result = co_await dd::this_coro::return_place;
                reqerr_t err = co_await api::send_request(request, client, bottoken, result, timeout);
                if (err) [[unlikely]] {{
                    TGBM_LOG_ERROR("{m.name} request ended with error, status: {{}}, description: {{}}", err.status, err.description.str());
                    handle_telegram_http_status(err.status);
                }}
                co_return dd::rvo;
            }}
            '''
            print(implstr, file=out)
    print('}\n', file=out) # namespace tgbm::api

def array_diff(arr1, arr2):
    set1 = set(arr1)
    set2 = set(arr2)
    diff = list((set1 - set2) | (set2 - set1))
    return diff

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--outdir", type=str, required=True, help="output dir for methods, must be tgbm/api dir")
    parser.add_argument("--apifile", type=str, required=True, help="file with loaded TG api (HTML) (without quotes)")

    args = parser.parse_args()

    print(f'[TGBM] creating directory: {args.outdir}\n')
    os.makedirs(args.outdir, exist_ok=True)

    methods = parse_methods(load_file(args.apifile))
    if len(methods) != len(G_OPERATION_TO_RESULT):
        print(array_diff([e.name for e in methods], G_OPERATION_TO_RESULT.keys()))
    generate_all_methods(methods, args.outdir)

    with open(f'{args.outdir}/../telegram.hpp', 'w', encoding='utf-8') as out:
        generate_api_fwd_header(methods, out)
    # methods / api / tgbm / include
    with open(f'{args.outdir}/../../../../src/telegram.cpp', 'w', encoding='utf-8') as out:
        generate_api_impl_file(methods, out)

if __name__ == '__main__':
    main()
