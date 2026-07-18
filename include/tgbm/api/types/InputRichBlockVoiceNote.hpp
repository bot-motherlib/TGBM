#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a block in a rich formatted message to be sent. Currently, it can be any of the
 * following types:*/
struct InputRichBlockVoiceNote {
  /* The voice note. Caption is ignored. */
  box<InputMediaVoiceNote> voice_note;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("voice_note", true).or_default(false);
  }
};

}  // namespace tgbm::api
