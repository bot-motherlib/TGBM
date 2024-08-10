#include "tgbm/types/ChatMemberOwner.h"
#include "tgbm/types/ChatMemberAdministrator.h"
#include "tgbm/types/ChatMemberMember.h"
#include "tgbm/types/ChatMemberRestricted.h"
#include "tgbm/types/ChatMemberLeft.h"
#include "tgbm/types/ChatMemberBanned.h"

#include <string>

using namespace tgbm;

const std::string ChatMemberOwner::STATUS = "creator";
const std::string ChatMemberAdministrator::STATUS = "administrator";
const std::string ChatMemberMember::STATUS = "member";
const std::string ChatMemberRestricted::STATUS = "restricted";
const std::string ChatMemberLeft::STATUS = "left";
const std::string ChatMemberBanned::STATUS = "kicked";
