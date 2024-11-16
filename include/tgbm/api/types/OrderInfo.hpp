#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents information about an order.*/
struct OrderInfo {
  /* Optional. User name */
  optional<String> name;
  /* Optional. User's phone number */
  optional<String> phone_number;
  /* Optional. User email */
  optional<String> email;
  /* Optional. User shipping address */
  box<ShippingAddress> shipping_address;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
