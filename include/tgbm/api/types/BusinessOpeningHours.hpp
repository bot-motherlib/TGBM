#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes the opening hours of a business.*/
struct BusinessOpeningHours {
  /* Unique name of the time zone for which the opening hours are defined */
  String time_zone_name;
  /* List of time intervals describing business opening hours */
  arrayof<BusinessOpeningHoursInterval> opening_hours;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("time_zone_name", true)
        .case_("opening_hours", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
