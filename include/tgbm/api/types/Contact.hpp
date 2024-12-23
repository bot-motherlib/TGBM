#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a phone contact.*/
struct Contact {
  /* Contact's phone number */
  String phone_number;
  /* Contact's first name */
  String first_name;
  /* Optional. Contact's last name */
  optional<String> last_name;
  /* Optional. Contact's user identifier in Telegram. This number may have more than 32 significant bits and
   * some programming languages may have difficulty/silent defects in interpreting it. But it has at most 52
   * significant bits, so a 64-bit integer or double-precision float type are safe for storing this
   * identifier. */
  optional<Integer> user_id;
  /* Optional. Additional data about the contact in the form of a vCard */
  optional<String> vcard;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("phone_number", true).case_("first_name", true).or_default(false);
  }
};

}  // namespace tgbm::api
