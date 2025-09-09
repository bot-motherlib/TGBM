#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about a payment refund for a suggested post.*/
struct SuggestedPostRefunded {
  /* Reason for the refund. Currently, one of “post_deleted” if the post was deleted within 24 hours of being
   * posted or removed from scheduled messages without being posted, or “payment_refunded” if the payer
   * refunded their payment. */
  String reason;
  /* Optional. Message containing the suggested post. Note that the Message object in this field will not
   * contain the reply_to_message field even if it itself is a reply. */
  box<Message> suggested_post_message;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("reason", true).or_default(false);
  }
};

}  // namespace tgbm::api
