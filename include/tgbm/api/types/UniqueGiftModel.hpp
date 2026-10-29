#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the model of a unique gift.*/
struct UniqueGiftModel {
  /* Name of the model */
  String name;
  /* The sticker that represents the unique gift */
  box<Sticker> sticker;
  /* The number of unique gifts that receive this model for every 1000 gift upgrades. Always 0 for crafted
   * gifts. */
  Integer rarity_per_mille;
  /* Optional. Rarity of the model if it is a crafted model. Currently, can be “uncommon”, “rare”, “epic”, or
   * “legendary”. */
  optional<String> rarity;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("name", true)
        .case_("sticker", true)
        .case_("rarity_per_mille", true)
        .or_default(false);
  }

  bool operator==(const UniqueGiftModel&) const;
  std::strong_ordering operator<=>(const UniqueGiftModel&) const;
};

}  // namespace tgbm::api
