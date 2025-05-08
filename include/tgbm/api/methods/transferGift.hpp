#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct transfer_gift_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Unique identifier of the regular gift that should be transferred */
  String owned_gift_id;
  /* Unique identifier of the chat which will own the gift. The chat must be active in the last 24 hours. */
  Integer new_owner_chat_id;
  /* The amount of Telegram Stars that will be paid for the transfer from the business account balance. If
   * positive, then the can_transfer_stars business bot right is required. */
  optional<Integer> star_count;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "transferGift";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("owned_gift_id", owned_gift_id);
    body.arg("new_owner_chat_id", new_owner_chat_id);
    if (star_count)
      body.arg("star_count", *star_count);
  }
};

}  // namespace tgbm::api
