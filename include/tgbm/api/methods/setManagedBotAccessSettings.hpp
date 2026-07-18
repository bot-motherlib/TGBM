#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct set_managed_bot_access_settings_request {
  /* User identifier of the managed bot whose access settings will be changed */
  Integer user_id;
  /* Pass True if only selected users can access the bot. The bot's owner can always access it. */
  bool is_access_restricted;
  /* A JSON-serialized list of up to 10 identifiers of users who will have access to the bot in addition to
   * its owner. Ignored if is_access_restricted is False. */
  optional<arrayof<Integer>> added_user_ids;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setManagedBotAccessSettings";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("is_access_restricted", is_access_restricted);
    if (added_user_ids)
      body.arg("added_user_ids", *added_user_ids);
  }
};

}  // namespace tgbm::api
