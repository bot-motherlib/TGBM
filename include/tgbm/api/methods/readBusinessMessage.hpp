#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct read_business_message_request {
  /* Unique identifier of the business connection on behalf of which to read the message */
  String business_connection_id;
  /* Unique identifier of the chat in which the message was received. The chat must have been active in the
   * last 24 hours. */
  Integer chat_id;
  /* Unique identifier of the message to mark as read */
  Integer message_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "readBusinessMessage";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("chat_id", chat_id);
    body.arg("message_id", message_id);
  }
};

}  // namespace tgbm::api
