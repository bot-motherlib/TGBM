#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the content of a message to be sent as a result of an inline query. Telegram clients
 * currently support the following 5 types:*/
struct InputContactMessageContent {
  /* Contact's first name */
  String first_name;
  /* Optional. Contact's last name */
  optional<String> last_name;
  /* Optional. Additional data about the contact in the form of a vCard, 0-2048 bytes */
  optional<String> vcard;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("first_name", true).or_default(false);
  }
};

}  // namespace tgbm::api
