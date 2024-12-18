#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct set_user_emoji_status_request {
  /* Unique identifier of the target user */
  Integer user_id;
  /* Custom emoji identifier of the emoji status to set. Pass an empty string to remove the status. */
  optional<String> emoji_status_custom_emoji_id;
  /* Expiration date of the emoji status, if any */
  optional<Integer> emoji_status_expiration_date;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setUserEmojiStatus";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    if (emoji_status_custom_emoji_id)
      body.arg("emoji_status_custom_emoji_id", *emoji_status_custom_emoji_id);
    if (emoji_status_expiration_date)
      body.arg("emoji_status_expiration_date", *emoji_status_expiration_date);
  }
};

}  // namespace tgbm::api
