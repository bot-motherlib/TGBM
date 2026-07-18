#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct send_chat_join_request_web_app_request {
  /* Unique identifier of the join request query */
  String chat_join_request_query_id;
  /* An HTTPS URL of a Web App to be opened with additional data as specified in Initializing Web Apps */
  String web_app_url;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "sendChatJoinRequestWebApp";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_join_request_query_id", chat_join_request_query_id);
    body.arg("web_app_url", web_app_url);
  }
};

}  // namespace tgbm::api
