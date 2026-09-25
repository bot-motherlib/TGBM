#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a story area pointing to a unique gift. Currently, a story can have at most 1 unique gift area.*/
struct StoryAreaTypeUniqueGift {
  /* Unique name of the gift */
  String name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("name", true).or_default(false);
  }
};

}  // namespace tgbm::api
