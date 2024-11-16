#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a story.*/
struct Story {
  /* Chat that posted the story */
  box<Chat> chat;
  /* Unique identifier for the story in the chat */
  Integer id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("chat", true).case_("id", true).or_default(false);
  }
};

}  // namespace tgbm::api
