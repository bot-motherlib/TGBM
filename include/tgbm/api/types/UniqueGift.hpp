#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes a unique gift that was upgraded from a regular gift.*/
struct UniqueGift {
  /* Human-readable name of the regular gift from which this unique gift was upgraded */
  String base_name;
  /* Unique name of the gift. This name can be used in https://t.me/nft/... links and story areas */
  String name;
  /* Unique number of the upgraded gift among gifts upgraded from the same regular gift */
  Integer number;
  /* Model of the gift */
  box<UniqueGiftModel> model;
  /* Symbol of the gift */
  box<UniqueGiftSymbol> symbol;
  /* Backdrop of the gift */
  box<UniqueGiftBackdrop> backdrop;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("base_name", true)
        .case_("name", true)
        .case_("number", true)
        .case_("model", true)
        .case_("symbol", true)
        .case_("backdrop", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
