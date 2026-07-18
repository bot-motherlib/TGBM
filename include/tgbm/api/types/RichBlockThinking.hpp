#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a block in a rich formatted message. Currently, it can be any of the following
 * types:*/
struct RichBlockThinking {
  /* Text of the block. See https://t.me/addemoji/AIActions for examples of custom emoji that are recommended
   * for usage in the block. */
  box<RichText> text;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
