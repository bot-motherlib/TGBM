#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains information about one answer option in a poll.*/
struct PollOption {
  /* Option text, 1-100 characters */
  String text;
  /* Number of users that voted for this option */
  Integer voter_count;
  /* Optional. Special entities that appear in the option text. Currently, only custom emoji entities are
   * allowed in poll option texts */
  optional<arrayof<MessageEntity>> text_entities;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("voter_count", true).or_default(false);
  }
};

}  // namespace tgbm::api
