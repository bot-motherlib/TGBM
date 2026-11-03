#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a story area pointing to an HTTP or tg:// link. Currently, a story can have up to 3 link areas.*/
struct StoryAreaTypeLink {
  /* HTTP or tg:// URL to be opened when the area is clicked */
  String url;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("url", true).or_default(false);
  }

  bool operator==(const StoryAreaTypeLink&) const;
  std::strong_ordering operator<=>(const StoryAreaTypeLink&) const;
};

}  // namespace tgbm::api
