#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents one shipping option.*/
struct ShippingOption {
  /* Shipping option identifier */
  String id;
  /* Option title */
  String title;
  /* List of price portions */
  arrayof<LabeledPrice> prices;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("id", true)
        .case_("title", true)
        .case_("prices", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
