#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes the type of a clickable area on a story. Currently, it can be one of*/
struct StoryAreaTypeLink {
  /* HTTP or tg:// URL to be opened when the area is clicked */
  String url;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("url", true).or_default(false);
  }
};

}  // namespace tgbm::api
