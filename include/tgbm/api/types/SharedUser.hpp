#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains information about a user that was shared with the bot using a
 * KeyboardButtonRequestUsers button.*/
struct SharedUser {
  /* Identifier of the shared user. This number may have more than 32 significant bits and some programming
   * languages may have difficulty/silent defects in interpreting it. But it has at most 52 significant bits,
   * so 64-bit integers or double-precision float types are safe for storing these identifiers. The bot may
   * not have access to the user and could be unable to use this identifier, unless the user is already known
   * to the bot by some other means. */
  Integer user_id;
  /* Optional. First name of the user, if the name was requested by the bot */
  optional<String> first_name;
  /* Optional. Last name of the user, if the name was requested by the bot */
  optional<String> last_name;
  /* Optional. Username of the user, if the username was requested by the bot */
  optional<String> username;
  /* Optional. Available sizes of the chat photo, if the photo was requested by the bot */
  optional<arrayof<PhotoSize>> photo;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("user_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
