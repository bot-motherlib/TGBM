#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a shipping address.*/
struct ShippingAddress {
  /* Two-letter ISO 3166-1 alpha-2 country code */
  String country_code;
  /* State, if applicable */
  String state;
  /* City */
  String city;
  /* First line for the address */
  String street_line1;
  /* Second line for the address */
  String street_line2;
  /* Address post code */
  String post_code;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("country_code", true)
        .case_("state", true)
        .case_("city", true)
        .case_("street_line1", true)
        .case_("street_line2", true)
        .case_("post_code", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
