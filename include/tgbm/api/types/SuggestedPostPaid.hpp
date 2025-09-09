#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about a successful payment for a suggested post.*/
struct SuggestedPostPaid {
  /* Currency in which the payment was made. Currently, one of “XTR” for Telegram Stars or “TON” for toncoins
   */
  String currency;
  /* Optional. Message containing the suggested post. Note that the Message object in this field will not
   * contain the reply_to_message field even if it itself is a reply. */
  box<Message> suggested_post_message;
  /* Optional. The amount of the currency that was received by the channel in nanotoncoins; for payments in
   * toncoins only */
  optional<Integer> amount;
  /* Optional. The amount of Telegram Stars that was received by the channel; for payments in Telegram Stars
   * only */
  box<StarAmount> star_amount;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("currency", true).or_default(false);
  }
};

}  // namespace tgbm::api
