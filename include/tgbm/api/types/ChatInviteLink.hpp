#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents an invite link for a chat.*/
struct ChatInviteLink {
  /* The invite link. If the link was created by another chat administrator, then the second part of the link
   * will be replaced with “…”. */
  String invite_link;
  /* Creator of the link */
  box<User> creator;
  /* True, if users joining the chat via the link need to be approved by chat administrators */
  bool creates_join_request;
  /* True, if the link is primary */
  bool is_primary;
  /* True, if the link is revoked */
  bool is_revoked;
  /* Optional. Invite link name */
  optional<String> name;
  /* Optional. Point in time (Unix timestamp) when the link will expire or has been expired */
  optional<Integer> expire_date;
  /* Optional. The maximum number of users that can be members of the chat simultaneously after joining the
   * chat via this invite link; 1-99999 */
  optional<Integer> member_limit;
  /* Optional. Number of pending join requests created using this link */
  optional<Integer> pending_join_request_count;
  /* Optional. The number of seconds the subscription will be active for before the next payment */
  optional<Integer> subscription_period;
  /* Optional. The amount of Telegram Stars a user must pay initially and after each subsequent subscription
   * period to be a member of the chat using the link */
  optional<Integer> subscription_price;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("invite_link", true)
        .case_("creator", true)
        .case_("creates_join_request", true)
        .case_("is_primary", true)
        .case_("is_revoked", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
