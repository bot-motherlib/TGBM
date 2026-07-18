#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about a chat being added to a community.*/
struct CommunityChatAdded {
  /* The new community to which the chat belongs */
  box<Community> community;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("community", true).or_default(false);
  }
};

}  // namespace tgbm::api
