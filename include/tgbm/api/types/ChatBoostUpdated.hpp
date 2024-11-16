#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a boost added to a chat or changed.*/
struct ChatBoostUpdated {
  /* Chat which was boosted */
  box<Chat> chat;
  /* Information about the chat boost */
  box<ChatBoost> boost;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("chat", true).case_("boost", true).or_default(false);
  }
};

}  // namespace tgbm::api
