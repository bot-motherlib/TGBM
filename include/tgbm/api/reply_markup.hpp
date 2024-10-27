#pragma once

#include "tgbm/tools/box_union.hpp"

namespace tgbm::api {

struct InlineKeyboardMarkup;
struct ReplyKeyboardMarkup;
struct ReplyKeyboardRemove;
struct ForceReply;

using reply_markup_t = box_union<InlineKeyboardMarkup, ReplyKeyboardMarkup, ReplyKeyboardRemove, ForceReply>;

}  // namespace tgbm::api
