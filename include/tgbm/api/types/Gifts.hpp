#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represent a list of gifts.*/
struct Gifts {
  /* The list of gifts */
  arrayof<Gift> gifts;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("gifts", true).or_default(false);
  }
};

}  // namespace tgbm::api
