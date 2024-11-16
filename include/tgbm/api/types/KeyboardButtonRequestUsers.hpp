#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object defines the criteria used to request suitable users. Information about the selected users will
 * be shared with the bot when the corresponding button is pressed. More about requesting users »*/
struct KeyboardButtonRequestUsers {
  /* Signed 32-bit identifier of the request that will be received back in the UsersShared object. Must be
   * unique within the message */
  Integer request_id;
  /* Optional. The maximum number of users to be selected; 1-10. Defaults to 1. */
  optional<Integer> max_quantity;
  /* Optional. Pass True to request bots, pass False to request regular users. If not specified, no additional
   * restrictions are applied. */
  optional<bool> user_is_bot;
  /* Optional. Pass True to request premium users, pass False to request non-premium users. If not specified,
   * no additional restrictions are applied. */
  optional<bool> user_is_premium;
  /* Optional. Pass True to request the users' first and last names */
  optional<bool> request_name;
  /* Optional. Pass True to request the users' usernames */
  optional<bool> request_username;
  /* Optional. Pass True to request the users' photos */
  optional<bool> request_photo;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("request_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
