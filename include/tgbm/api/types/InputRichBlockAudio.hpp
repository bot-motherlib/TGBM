#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with a music file, corresponding to the HTML tag <audio>.*/
struct InputRichBlockAudio {
  /* The audio. Caption is ignored. */
  box<InputMediaAudio> audio;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("audio", true).or_default(false);
  }
};

}  // namespace tgbm::api
