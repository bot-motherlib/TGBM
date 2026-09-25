#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with a “Thinking…” placeholder, corresponding to the custom HTML tag <tg-thinking>. The block may be
 * used only in sendRichMessageDraft, therefore it can't be received in messages. See
 * https://t.me/addemoji/AIActions for examples of custom emoji that are recommended for usage in the block.*/
struct InputRichBlockThinking {
  /* Text of the block. See https://t.me/addemoji/AIActions for examples of custom emoji that are recommended
   * for usage in the block. */
  box<RichText> text;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
