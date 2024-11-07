#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct set_webhook_request {
  /* HTTPS URL to send updates to. Use an empty string to remove webhook integration */
  String url;
  /* Upload your public key certificate so that the root certificate in use can be checked. See our
   * self-signed guide for details. */
  optional<InputFile> certificate;
  /* The fixed IP address which will be used to send webhook requests instead of the IP address resolved
   * through DNS */
  optional<String> ip_address;
  /* The maximum allowed number of simultaneous HTTPS connections to the webhook for update delivery, 1-100.
   * Defaults to 40. Use lower values to limit the load on your bot's server, and higher values to increase
   * your bot's throughput. */
  optional<Integer> max_connections;
  /* A JSON-serialized list of the update types you want your bot to receive. For example, specify ["message",
   * "edited_channel_post", "callback_query"] to only receive updates of these types. See Update for a
   * complete list of available update types. Specify an empty list to receive all update types except
   * chat_member, message_reaction, and message_reaction_count (default). If not specified, the previous
   * setting will be used.Please note that this parameter doesn't affect updates created before the call to
   * the setWebhook, so unwanted updates may be received for a short period of time. */
  optional<arrayof<String>> allowed_updates;
  /* Pass True to drop all pending updates */
  optional<bool> drop_pending_updates;
  /* A secret token to be sent in a header “X-Telegram-Bot-Api-Secret-Token” in every webhook request, 1-256
   * characters. Only characters A-Z, a-z, 0-9, _ and - are allowed. The header is useful to ensure that the
   * request comes from a webhook set by you. */
  optional<String> secret_token;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::yes;
  static constexpr std::string_view api_method_name = "setWebhook";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("url", url);
    if (ip_address)
      body.arg("ip_address", *ip_address);
    if (max_connections)
      body.arg("max_connections", *max_connections);
    if (allowed_updates)
      body.arg("allowed_updates", *allowed_updates);
    if (drop_pending_updates)
      body.arg("drop_pending_updates", *drop_pending_updates);
    if (secret_token)
      body.arg("secret_token", *secret_token);
  }

  [[nodiscard]] bool has_file_args() const noexcept {
    if (certificate)
      return true;
    return false;
  }

  void fill_file_args(application_multipart_form_data& body) const {
    if (certificate)
      body.arg("certificate", *certificate);
  }
};

}  // namespace tgbm::api
