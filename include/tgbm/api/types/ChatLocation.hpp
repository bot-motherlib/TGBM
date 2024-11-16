#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Represents a location to which a chat is connected.*/
struct ChatLocation {
  /* The location to which the supergroup is connected. Can't be a live location. */
  box<Location> location;
  /* Location address; 1-64 characters, as defined by the chat owner */
  String address;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("location", true).case_("address", true).or_default(false);
  }
};

}  // namespace tgbm::api
