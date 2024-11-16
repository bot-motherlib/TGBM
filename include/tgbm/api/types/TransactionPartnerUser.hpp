#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes the source of a transaction, or its recipient for outgoing transactions. Currently, it
 * can be one of*/
struct TransactionPartnerUser {
  /* Information about the user */
  box<User> user;
  /* Optional. Bot-specified invoice payload */
  optional<String> invoice_payload;
  /* Optional. Information about the paid media bought by the user */
  optional<arrayof<PaidMedia>> paid_media;
  /* Optional. Bot-specified paid media payload */
  optional<String> paid_media_payload;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("user", true).or_default(false);
  }
};

}  // namespace tgbm::api
