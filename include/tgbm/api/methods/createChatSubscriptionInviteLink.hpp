#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/ChatInviteLink.hpp"

namespace tgbm::api {

struct create_chat_subscription_invite_link_request {
  /* Unique identifier for the target channel chat or username of the target channel (in the format
   * @channelusername) */
  int_or_str chat_id;
  /* The number of seconds the subscription will be active for before the next payment. Currently, it must
   * always be 2592000 (30 days). */
  Integer subscription_period;
  /* The amount of Telegram Stars a user must pay initially and after each subsequent subscription period to
   * be a member of the chat; 1-2500 */
  Integer subscription_price;
  /* Invite link name; 0-32 characters */
  optional<String> name;

  using return_type = ChatInviteLink;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "createChatSubscriptionInviteLink";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    if (name)
      body.arg("name", *name);
    body.arg("subscription_period", subscription_period);
    body.arg("subscription_price", subscription_price);
  }
};

}  // namespace tgbm::api
