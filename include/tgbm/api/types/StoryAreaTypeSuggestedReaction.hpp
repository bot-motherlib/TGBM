#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes the type of a clickable area on a story. Currently, it can be one of*/
struct StoryAreaTypeSuggestedReaction {
  /* Type of the reaction */
  box<ReactionType> reaction_type;
  /* Optional. Pass True if the reaction area has a dark background */
  optional<bool> is_dark;
  /* Optional. Pass True if reaction area corner is flipped */
  optional<bool> is_flipped;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("reaction_type", true).or_default(false);
  }
};

}  // namespace tgbm::api
