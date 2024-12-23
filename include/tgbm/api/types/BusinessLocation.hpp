#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Contains information about the location of a Telegram Business account.*/
struct BusinessLocation {
  /* Address of the business */
  String address;
  /* Optional. Location of the business */
  box<Location> location;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("address", true).or_default(false);
  }
};

}  // namespace tgbm::api
