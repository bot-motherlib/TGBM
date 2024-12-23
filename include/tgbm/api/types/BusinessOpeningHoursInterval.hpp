#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes an interval of time during which a business is open.*/
struct BusinessOpeningHoursInterval {
  /* The minute's sequence number in a week, starting on Monday, marking the start of the time interval during
   * which the business is open; 0 - 7 * 24 * 60 */
  Integer opening_minute;
  /* The minute's sequence number in a week, starting on Monday, marking the end of the time interval during
   * which the business is open; 0 - 8 * 24 * 60 */
  Integer closing_minute;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("opening_minute", true)
        .case_("closing_minute", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
