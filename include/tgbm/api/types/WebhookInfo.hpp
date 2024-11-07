#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Describes the current status of a webhook.*/
struct WebhookInfo {
  /* Webhook URL, may be empty if webhook is not set up */
  String url;
  /* True, if a custom certificate was provided for webhook certificate checks */
  bool has_custom_certificate;
  /* Number of updates awaiting delivery */
  Integer pending_update_count;
  /* Optional. Currently used webhook IP address */
  optional<String> ip_address;
  /* Optional. Unix time for the most recent error that happened when trying to deliver an update via webhook
   */
  optional<Integer> last_error_date;
  /* Optional. Error message in human-readable format for the most recent error that happened when trying to
   * deliver an update via webhook */
  optional<String> last_error_message;
  /* Optional. Unix time of the most recent error that happened when trying to synchronize available updates
   * with Telegram datacenters */
  optional<Integer> last_synchronization_error_date;
  /* Optional. The maximum allowed number of simultaneous HTTPS connections to the webhook for update delivery
   */
  optional<Integer> max_connections;
  /* Optional. A list of update types the bot is subscribed to. Defaults to all update types except
   * chat_member */
  optional<arrayof<String>> allowed_updates;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("url", true)
        .case_("has_custom_certificate", true)
        .case_("pending_update_count", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
