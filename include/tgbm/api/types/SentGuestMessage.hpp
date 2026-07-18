#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes an inline message sent by a guest bot.*/
struct SentGuestMessage {
  /* Identifier of the sent inline message */
  String inline_message_id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("inline_message_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
