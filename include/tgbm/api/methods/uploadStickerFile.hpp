#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/File.hpp>

namespace tgbm::api {

struct upload_sticker_file_request {
  /* User identifier of sticker file owner */
  Integer user_id;
  /* A file with the sticker in .WEBP, .PNG, .TGS, or .WEBM format. See https://core.telegram.org/stickers for
   * technical requirements. More information on Sending Files » */
  InputFile sticker;
  /* Format of the sticker, must be one of “static”, “animated”, “video” */
  String sticker_format;

  using return_type = File;
  static constexpr file_info_e file_info = file_info_e::yes;
  static constexpr std::string_view api_method_name = "uploadStickerFile";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("sticker_format", sticker_format);
  }

  [[nodiscard]] bool has_file_args() const noexcept {
    return true;
  }

  void fill_file_args(application_multipart_form_data& body) const {
    body.arg("sticker", sticker);
  }
};

}  // namespace tgbm::api
