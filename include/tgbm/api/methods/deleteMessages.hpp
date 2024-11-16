#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct delete_messages_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* A JSON-serialized list of 1-100 identifiers of messages to delete. See deleteMessage for limitations on
   * which messages can be deleted */
  arrayof<Integer> message_ids;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "deleteMessages";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("message_ids", message_ids);
  }
};

}  // namespace tgbm::api
