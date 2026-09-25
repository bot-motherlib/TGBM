#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A hashtag.*/
struct RichTextHashtag {
  /* The text */
  box<RichText> text;
  /* The hashtag */
  String hashtag;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("hashtag", true).or_default(false);
  }
};

}  // namespace tgbm::api
