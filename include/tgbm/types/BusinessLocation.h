#ifndef TGBOT_BUSINESSLOCATION_H
#define TGBOT_BUSINESSLOCATION_H

#include "tgbm/types/Location.h"

#include <memory>
#include <string>

namespace tgbm {

/**
 * @ingroup types
 */
class BusinessLocation {
 public:
  typedef std::shared_ptr<BusinessLocation> Ptr;

  /**
   * @brief Address of the business
   */
  std::string address;

  /**
   * @brief Optional. Location of the business
   */
  Location::Ptr location;
};
}  // namespace tgbm

#endif  // TGBOT_BUSINESSLOCATION_H
