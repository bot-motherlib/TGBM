#include "tgbm/types/BotCommandScopeDefault.h"
#include "tgbm/types/BotCommandScopeAllPrivateChats.h"
#include "tgbm/types/BotCommandScopeAllGroupChats.h"
#include "tgbm/types/BotCommandScopeAllChatAdministrators.h"
#include "tgbm/types/BotCommandScopeChat.h"
#include "tgbm/types/BotCommandScopeChatAdministrators.h"
#include "tgbm/types/BotCommandScopeChatMember.h"

#include <string>

using namespace tgbm;

const std::string BotCommandScopeDefault::TYPE = "default";
const std::string BotCommandScopeAllPrivateChats::TYPE = "all_private_chats";
const std::string BotCommandScopeAllGroupChats::TYPE = "all_group_chats";
const std::string BotCommandScopeAllChatAdministrators::TYPE = "all_chat_administrators";
const std::string BotCommandScopeChat::TYPE = "chat";
const std::string BotCommandScopeChatAdministrators::TYPE = "chat_administrators";
const std::string BotCommandScopeChatMember::TYPE = "chat_member";
