#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains information about a chat boost.*/
struct ChatBoost {
  /* Unique identifier of the boost */
  String boost_id;
  /* Point in time (Unix timestamp) when the chat was boosted */
  Integer add_date;
  /* Point in time (Unix timestamp) when the boost will automatically expire, unless the booster's Telegram
   * Premium subscription is prolonged */
  Integer expiration_date;
  /* Source of the added boost */
  box<ChatBoostSource> source;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("boost_id", true)
        .case_("add_date", true)
        .case_("expiration_date", true)
        .case_("source", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
