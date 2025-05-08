#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the backdrop of a unique gift.*/
struct UniqueGiftBackdrop {
  /* Name of the backdrop */
  String name;
  /* Colors of the backdrop */
  box<UniqueGiftBackdropColors> colors;
  /* The number of unique gifts that receive this backdrop for every 1000 gifts upgraded */
  Integer rarity_per_mille;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("name", true)
        .case_("colors", true)
        .case_("rarity_per_mille", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
