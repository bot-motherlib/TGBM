#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Describes a Web App.*/
struct WebAppInfo {
  /* An HTTPS URL of a Web App to be opened with additional data as specified in Initializing Web Apps */
  String url;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("url", true).or_default(false);
  }
};

}  // namespace tgbm::api
