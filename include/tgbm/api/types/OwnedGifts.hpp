#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Contains the list of gifts received and owned by a user or a chat.*/
struct OwnedGifts {
  /* The total number of gifts owned by the user or the chat */
  Integer total_count;
  /* The list of gifts */
  arrayof<OwnedGift> gifts;
  /* Optional. Offset for the next request. If empty, then there are no more results */
  optional<String> next_offset;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("total_count", true).case_("gifts", true).or_default(false);
  }
};

}  // namespace tgbm::api
