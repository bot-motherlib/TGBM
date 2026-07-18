#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents an HTTP link.*/
struct Link {
  /* URL of the link */
  String url;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("url", true).or_default(false);
  }
};

}  // namespace tgbm::api
