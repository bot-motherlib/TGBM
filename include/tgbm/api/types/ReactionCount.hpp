#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents a reaction added to a message along with the number of times it was added.*/
struct ReactionCount {
  /* Type of the reaction */
  box<ReactionType> type;
  /* Number of times the reaction was added */
  Integer total_count;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("type", true).case_("total_count", true).or_default(false);
  }
};

}  // namespace tgbm::api
