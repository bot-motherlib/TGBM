#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents an answer of a user in a non-anonymous poll.*/
struct PollAnswer {
  /* Unique poll identifier */
  String poll_id;
  /* 0-based identifiers of chosen answer options. May be empty if the vote was retracted. */
  arrayof<Integer> option_ids;
  /* Optional. The chat that changed the answer to the poll, if the voter is anonymous */
  box<Chat> voter_chat;
  /* Optional. The user that changed the answer to the poll, if the voter isn't anonymous */
  box<User> user;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("poll_id", true).case_("option_ids", true).or_default(false);
  }
};

}  // namespace tgbm::api
