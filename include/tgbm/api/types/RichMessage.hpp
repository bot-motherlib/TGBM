#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Rich formatted message.*/
struct RichMessage {
  /* Content of the message */
  arrayof<RichBlock> blocks;
  /* Optional. True, if the rich message must be shown right-to-left */
  optional<bool> is_rtl;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("blocks", true).or_default(false);
  }
};

}  // namespace tgbm::api
