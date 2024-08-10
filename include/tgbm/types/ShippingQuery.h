#pragma once

#include "tgbm/types/User.h"
#include "tgbm/types/ShippingAddress.h"

#include <string>
#include <memory>
#include <vector>

namespace tgbm {

/**
 * @brief This object contains information about an incoming shipping query.
 *
 * https://core.telegram.org/bots/api#shippingquery
 *
 * @ingroup types
 */
class ShippingQuery {
 public:
  typedef std::shared_ptr<ShippingQuery> Ptr;

  /**
   * @brief Unique query identifier.
   */
  std::string id;

  /**
   * @brief User who sent the query.
   */
  User::Ptr from;

  /**
   * @brief Bot specified invoice payload.
   */
  std::string invoicePayload;

  /**
   * @brief User specified shipping address.
   */
  ShippingAddress::Ptr shippingAddress;
};
}  // namespace tgbm
