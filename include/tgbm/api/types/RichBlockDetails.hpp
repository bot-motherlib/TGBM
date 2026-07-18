#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a block in a rich formatted message. Currently, it can be any of the following
 * types:*/
struct RichBlockDetails {
  /* Always shown summary of the block */
  box<RichText> summary;
  /* Content of the block */
  arrayof<RichBlock> blocks;
  /* Optional. True, if the content of the block is visible by default */
  optional<True> is_open;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("summary", true).case_("blocks", true).or_default(false);
  }
};

}  // namespace tgbm::api
