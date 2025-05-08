#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes the type of a clickable area on a story. Currently, it can be one of*/
struct StoryAreaTypeUniqueGift {
  /* Unique name of the gift */
  String name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("name", true).or_default(false);
  }
};

}  // namespace tgbm::api
