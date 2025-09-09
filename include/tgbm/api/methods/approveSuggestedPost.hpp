#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct approve_suggested_post_request {
  /* Unique identifier for the target direct messages chat */
  Integer chat_id;
  /* Identifier of a suggested post message to approve */
  Integer message_id;
  /* Point in time (Unix timestamp) when the post is expected to be published; omit if the date has already
   * been specified when the suggested post was created. If specified, then the date must be not more than
   * 2678400 seconds (30 days) in the future */
  optional<Integer> send_date;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "approveSuggestedPost";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("message_id", message_id);
    if (send_date)
      body.arg("send_date", *send_date);
  }
};

}  // namespace tgbm::api
