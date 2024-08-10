#include "tgbm/types/MessageOriginUser.h"
#include "tgbm/types/MessageOriginHiddenUser.h"
#include "tgbm/types/MessageOriginChat.h"
#include "tgbm/types/MessageOriginChannel.h"

#include <string>

using namespace tgbm;

const std::string MessageOriginUser::TYPE = "user";
const std::string MessageOriginHiddenUser::TYPE = "hidden_user";
const std::string MessageOriginChat::TYPE = "chat";
const std::string MessageOriginChannel::TYPE = "channel";
