#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the type of a background. Currently, it can be one of*/
struct BackgroundTypeChatTheme {
  /* Name of the chat theme, which is usually an emoji */
  String theme_name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("theme_name", true).or_default(false);
  }
};

}  // namespace tgbm::api
