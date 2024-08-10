#pragma once

#include "tgbm/types/BusinessOpeningHoursInterval.h"

#include <memory>
#include <string>
#include <vector>

namespace tgbm {

/**
 * @ingroup types
 */
class BusinessOpeningHours {
 public:
  typedef std::shared_ptr<BusinessOpeningHours> Ptr;

  /**
   * @brief Unique name of the time zone for which the opening hours are defined
   */
  std::string timeZoneName;

  /**
   * @brief List of time intervals describing business opening hours
   */
  std::vector<BusinessOpeningHoursInterval::Ptr> openingHours;
};
}  // namespace tgbm
