#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The reaction is based on a custom emoji.*/
struct ReactionTypeCustomEmoji {
  /* Custom emoji identifier */
  String custom_emoji_id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("custom_emoji_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
