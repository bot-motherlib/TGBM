#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct upgrade_gift_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Unique identifier of the regular gift that should be upgraded to a unique one */
  String owned_gift_id;
  /* Pass True to keep the original gift text, sender and receiver in the upgraded gift */
  optional<bool> keep_original_details;
  /* The amount of Telegram Stars that will be paid for the upgrade from the business account balance. If
   * gift.prepaid_upgrade_star_count > 0, then pass 0, otherwise, the can_transfer_stars business bot right is
   * required and gift.upgrade_star_count must be passed. */
  optional<Integer> star_count;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "upgradeGift";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("owned_gift_id", owned_gift_id);
    if (keep_original_details)
      body.arg("keep_original_details", *keep_original_details);
    if (star_count)
      body.arg("star_count", *star_count);
  }
};

}  // namespace tgbm::api
