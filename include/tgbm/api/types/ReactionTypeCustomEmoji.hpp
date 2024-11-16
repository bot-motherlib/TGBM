#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the type of a reaction. Currently, it can be one of*/
struct ReactionTypeCustomEmoji {
  /* Custom emoji identifier */
  String custom_emoji_id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("custom_emoji_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
