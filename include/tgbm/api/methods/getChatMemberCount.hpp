#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct get_chat_member_count_request {
  /* Unique identifier for the target chat or username of the target supergroup or channel (in the format
   * @channelusername) */
  int_or_str chat_id;

  using return_type = Integer;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getChatMemberCount";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
  }
};

}  // namespace tgbm::api
