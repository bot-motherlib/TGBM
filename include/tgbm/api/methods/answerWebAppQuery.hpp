#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InlineQueryResult.hpp>
#include <tgbm/api/types/SentWebAppMessage.hpp>

namespace tgbm::api {

struct answer_web_app_query_request {
  /* Unique identifier for the query to be answered */
  String web_app_query_id;
  /* A JSON-serialized object describing the message to be sent */
  box<InlineQueryResult> result;

  using return_type = SentWebAppMessage;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "answerWebAppQuery";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("web_app_query_id", web_app_query_id);
    body.arg("result", result);
  }
};

}  // namespace tgbm::api
