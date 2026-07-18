#pragma once

#include <tgbm/utils/box_union.hpp>

namespace tgbm::api {

struct InlineKeyboardMarkup;
struct ReplyKeyboardMarkup;
struct ReplyKeyboardRemove;
struct ForceReply;

using reply_markup_t = box_union<InlineKeyboardMarkup, ReplyKeyboardMarkup, ReplyKeyboardRemove, ForceReply>;

struct InputMediaAnimation;
struct InputMediaAudio;
struct InputMediaPhoto;
struct InputMediaVideo;
struct InputMediaVoiceNote;

using input_media_t =
    box_union<InputMediaAnimation, InputMediaAudio, InputMediaPhoto, InputMediaVideo, InputMediaVoiceNote>;

}  // namespace tgbm::api
