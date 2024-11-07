#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/Update.hpp"

namespace tgbm::api {

struct get_updates_request {
  /* Identifier of the first update to be returned. Must be greater by one than the highest among the
   * identifiers of previously received updates. By default, updates starting with the earliest unconfirmed
   * update are returned. An update is considered confirmed as soon as getUpdates is called with an offset
   * higher than its update_id. The negative offset can be specified to retrieve updates starting from -offset
   * update from the end of the updates queue. All previous updates will be forgotten. */
  optional<Integer> offset;
  /* Limits the number of updates to be retrieved. Values between 1-100 are accepted. Defaults to 100. */
  optional<Integer> limit;
  /* Timeout in seconds for long polling. Defaults to 0, i.e. usual short polling. Should be positive, short
   * polling should be used for testing purposes only. */
  optional<Integer> timeout;
  /* A JSON-serialized list of the update types you want your bot to receive. For example, specify ["message",
   * "edited_channel_post", "callback_query"] to only receive updates of these types. See Update for a
   * complete list of available update types. Specify an empty list to receive all update types except
   * chat_member, message_reaction, and message_reaction_count (default). If not specified, the previous
   * setting will be used.Please note that this parameter doesn't affect updates created before the call to
   * the getUpdates, so unwanted updates may be received for a short period of time. */
  optional<arrayof<String>> allowed_updates;

  using return_type = arrayof<Update>;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getUpdates";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (offset)
      body.arg("offset", *offset);
    if (limit)
      body.arg("limit", *limit);
    if (timeout)
      body.arg("timeout", *timeout);
    if (allowed_updates)
      body.arg("allowed_updates", *allowed_updates);
  }
};

}  // namespace tgbm::api
