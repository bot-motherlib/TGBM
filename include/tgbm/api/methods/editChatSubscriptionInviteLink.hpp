#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/ChatInviteLink.hpp>

namespace tgbm::api {

struct edit_chat_subscription_invite_link_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* The invite link to edit */
  String invite_link;
  /* Invite link name; 0-32 characters */
  optional<String> name;

  using return_type = ChatInviteLink;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editChatSubscriptionInviteLink";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("invite_link", invite_link);
    if (name)
      body.arg("name", *name);
  }
};

}  // namespace tgbm::api
