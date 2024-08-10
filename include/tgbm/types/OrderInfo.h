#pragma once

#include "tgbm/types/ShippingAddress.h"

#include <string>
#include <memory>

namespace tgbm {

/**
 * @brief This object represents information about an order.
 *
 * https://core.telegram.org/bots/api#orderinfo
 *
 * @ingroup types
 */
class OrderInfo {
 public:
  typedef std::shared_ptr<OrderInfo> Ptr;

  /**
   * @brief Optional. User name.
   */
  std::string name;

  /**
   * @brief Optional. User's phone number.
   */
  std::string phoneNumber;

  /**
   * @brief Optional. User email.
   */
  std::string email;

  /**
   * @brief Optional. User shipping address.
   */
  ShippingAddress::Ptr shippingAddress;
};

}  // namespace tgbm
