#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the content of a poll option to be sent. It should be one of*/
struct InputMediaLink {
  /* HTTP URL of the link */
  String url;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("url", true).or_default(false);
  }
};

}  // namespace tgbm::api
