#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about one answer option in a poll.*/
struct PollOption {
  /* Unique identifier of the option, persistent on option addition and deletion */
  String persistent_id;
  /* Option text, 1-100 characters */
  String text;
  /* Number of users who voted for this option; may be 0 if unknown */
  Integer voter_count;
  /* Optional. Special entities that appear in the option text. Currently, only custom emoji entities are
   * allowed in poll option texts */
  optional<arrayof<MessageEntity>> text_entities;
  /* Optional. User who added the option; omitted if the option wasn't added by a user after poll creation */
  box<User> added_by_user;
  /* Optional. Chat that added the option; omitted if the option wasn't added by a chat after poll creation */
  box<Chat> added_by_chat;
  /* Optional. Point in time (Unix timestamp) when the option was added; omitted if the option existed in the
   * original poll */
  optional<Integer> addition_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("persistent_id", true)
        .case_("text", true)
        .case_("voter_count", true)
        .or_default(false);
  }

  bool operator==(const PollOption&) const;
  std::strong_ordering operator<=>(const PollOption&) const;
};

}  // namespace tgbm::api
