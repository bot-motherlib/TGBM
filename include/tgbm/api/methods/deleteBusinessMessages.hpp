#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct delete_business_messages_request {
  /* Unique identifier of the business connection on behalf of which to delete the messages */
  String business_connection_id;
  /* A JSON-serialized list of 1-100 identifiers of messages to delete. All messages must be from the same
   * chat. See deleteMessage for limitations on which messages can be deleted */
  arrayof<Integer> message_ids;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "deleteBusinessMessages";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("message_ids", message_ids);
  }
};

}  // namespace tgbm::api
