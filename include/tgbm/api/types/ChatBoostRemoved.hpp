#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a boost removed from a chat.*/
struct ChatBoostRemoved {
  /* Chat which was boosted */
  box<Chat> chat;
  /* Unique identifier of the boost */
  String boost_id;
  /* Point in time (Unix timestamp) when the boost was removed */
  Integer remove_date;
  /* Source of the removed boost */
  box<ChatBoostSource> source;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("chat", true)
        .case_("boost_id", true)
        .case_("remove_date", true)
        .case_("source", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
