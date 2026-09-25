#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The reaction is based on an emoji.*/
struct ReactionTypeEmoji {
  /* Reaction emoji. Currently, it can be one of "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
   * "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
   * "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
   * "", "", "", "", "", "". */
  String emoji;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("emoji", true).or_default(false);
  }
};

}  // namespace tgbm::api
