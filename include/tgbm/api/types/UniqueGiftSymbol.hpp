#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the symbol shown on the pattern of a unique gift.*/
struct UniqueGiftSymbol {
  /* Name of the symbol */
  String name;
  /* The sticker that represents the unique gift */
  box<Sticker> sticker;
  /* The number of unique gifts that receive this model for every 1000 gifts upgraded */
  Integer rarity_per_mille;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("name", true)
        .case_("sticker", true)
        .case_("rarity_per_mille", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
