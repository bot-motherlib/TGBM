#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the source of a transaction, or its recipient for outgoing transactions. Currently, it
 * can be one of*/
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
