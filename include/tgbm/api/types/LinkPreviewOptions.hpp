#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Describes the options used for link preview generation.*/
struct LinkPreviewOptions {
  /* Optional. URL to use for the link preview. If empty, then the first URL found in the message text will be
   * used */
  optional<String> url;
  /* Optional. True, if the link preview is disabled */
  optional<bool> is_disabled;
  /* Optional. True, if the media in the link preview is supposed to be shrunk; ignored if the URL isn't
   * explicitly specified or media size change isn't supported for the preview */
  optional<bool> prefer_small_media;
  /* Optional. True, if the media in the link preview is supposed to be enlarged; ignored if the URL isn't
   * explicitly specified or media size change isn't supported for the preview */
  optional<bool> prefer_large_media;
  /* Optional. True, if the link preview must be shown above the message text; otherwise, the link preview
   * will be shown below the message text */
  optional<bool> show_above_text;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
