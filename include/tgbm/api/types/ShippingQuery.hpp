#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about an incoming shipping query.*/
struct ShippingQuery {
  /* Unique query identifier */
  String id;
  /* User who sent the query */
  box<User> from;
  /* Bot-specified invoice payload */
  String invoice_payload;
  /* User specified shipping address */
  box<ShippingAddress> shipping_address;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("from", true)
        .case_("invoice_payload", true)
        .case_("shipping_address", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
