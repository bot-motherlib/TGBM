#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a transaction with a chat.*/
struct TransactionPartnerChat {
  /* Information about the chat */
  box<Chat> chat;
  /* Optional. The gift sent to the chat by the bot */
  box<Gift> gift;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("chat", true).or_default(false);
  }
};

}  // namespace tgbm::api
