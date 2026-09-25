#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a story area containing weather information. Currently, a story can have up to 3 weather areas.*/
struct StoryAreaTypeWeather {
  /* Temperature, in degree Celsius */
  Double temperature;
  /* Emoji representing the weather */
  String emoji;
  /* A color of the area background in the ARGB format */
  Integer background_color;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("temperature", true)
        .case_("emoji", true)
        .case_("background_color", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
