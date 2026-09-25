#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a story area pointing to a location. Currently, a story can have up to 10 location areas.*/
struct StoryAreaTypeLocation {
  /* Location latitude in degrees */
  Double latitude;
  /* Location longitude in degrees */
  Double longitude;
  /* Optional. Address of the location */
  box<LocationAddress> address;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("latitude", true).case_("longitude", true).or_default(false);
  }
};

}  // namespace tgbm::api
