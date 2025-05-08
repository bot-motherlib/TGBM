#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes the type of a clickable area on a story. Currently, it can be one of*/
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
