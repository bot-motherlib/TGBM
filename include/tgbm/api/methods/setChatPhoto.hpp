#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct set_chat_photo_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* New chat photo, uploaded using multipart/form-data */
  InputFile photo;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::yes;
  static constexpr std::string_view api_method_name = "setChatPhoto";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
  }

  [[nodiscard]] bool has_file_args() const noexcept {
    return true;
  }

  void fill_file_args(application_multipart_form_data& body) const {
    body.arg("photo", photo);
  }
};

}  // namespace tgbm::api
