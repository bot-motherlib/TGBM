#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a media element embedded in an outgoing rich message.*/
struct InputRichMessageMedia {
  /* Unique identifier of the media used in a tg://photo?id=, tg://video?id=, or tg://audio?id= link. 1-64
   * characters, only A-Z, a-z, 0-9, _ and - are allowed. */
  String id;
  /* The media to be sent. Everything except the media itself and its properties is ignored. */
  input_media_t media;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("id", true).case_("media", true).or_default(false);
  }
};

}  // namespace tgbm::api
