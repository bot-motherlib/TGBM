#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct set_sticker_set_thumbnail_request {
  /* Sticker set name */
  String name;
  /* User identifier of the sticker set owner */
  Integer user_id;
  /* Format of the thumbnail, must be one of “static” for a .WEBP or .PNG image, “animated” for a .TGS
   * animation, or “video” for a .WEBM video */
  String format;
  /* A .WEBP or .PNG image with the thumbnail, must be up to 128 kilobytes in size and have a width and height
   * of exactly 100px, or a .TGS animation with a thumbnail up to 32 kilobytes in size (see
   * https://core.telegram.org/stickers#animation-requirements for animated sticker technical requirements),
   * or a .WEBM video with the thumbnail up to 32 kilobytes in size; see
   * https://core.telegram.org/stickers#video-requirements for video sticker technical requirements. Pass a
   * file_id as a String to send a file that already exists on the Telegram servers, pass an HTTP URL as a
   * String for Telegram to get a file from the Internet, or upload a new one using multipart/form-data. More
   * information on Sending Files ». Animated and video sticker set thumbnails can't be uploaded via HTTP URL.
   * If omitted, then the thumbnail is dropped and the first sticker is used as the thumbnail. */
  optional<file_or_str> thumbnail;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::maybe;
  static constexpr std::string_view api_method_name = "setStickerSetThumbnail";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("name", name);
    body.arg("user_id", user_id);
    if (thumbnail)
      if (const auto* str = thumbnail->get_str())
        body.arg("thumbnail", *str);
    body.arg("format", format);
  }

  [[nodiscard]] bool has_file_args() const noexcept {
    if (thumbnail && thumbnail->is_file())
      return true;
    return false;
  }

  void fill_file_args(application_multipart_form_data& body) const {
    if (thumbnail)
      if (const InputFile* f = thumbnail->get_file())
        body.arg("thumbnail", *f);
  }
};

}  // namespace tgbm::api
