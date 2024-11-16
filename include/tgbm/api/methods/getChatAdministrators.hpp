#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/ChatMember.hpp>

namespace tgbm::api {

struct get_chat_administrators_request {
  /* Unique identifier for the target chat or username of the target supergroup or channel (in the format
   * @channelusername) */
  int_or_str chat_id;

  using return_type = arrayof<ChatMember>;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getChatAdministrators";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
  }
};

}  // namespace tgbm::api
