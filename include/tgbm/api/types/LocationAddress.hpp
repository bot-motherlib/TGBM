#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes the physical address of a location.*/
struct LocationAddress {
  /* The two-letter ISO 3166-1 alpha-2 country code of the country where the location is located */
  String country_code;
  /* Optional. State of the location */
  optional<String> state;
  /* Optional. City of the location */
  optional<String> city;
  /* Optional. Street address of the location */
  optional<String> street;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("country_code", true).or_default(false);
  }
};

}  // namespace tgbm::api
