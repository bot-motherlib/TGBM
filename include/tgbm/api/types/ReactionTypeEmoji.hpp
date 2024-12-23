#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the type of a reaction. Currently, it can be one of*/
struct ReactionTypeEmoji {
  /* Reaction emoji. Currently, it can be one of "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
   * "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
   * "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
   * "", "", "", "", "", "" */
  String emoji;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("emoji", true).or_default(false);
  }
};

}  // namespace tgbm::api
