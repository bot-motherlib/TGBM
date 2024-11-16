#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about a poll.*/
struct Poll {
  /* Unique poll identifier */
  String id;
  /* Poll question, 1-300 characters */
  String question;
  /* List of poll options */
  arrayof<PollOption> options;
  /* Total number of users that voted in the poll */
  Integer total_voter_count;
  /* True, if the poll is closed */
  bool is_closed;
  /* True, if the poll is anonymous */
  bool is_anonymous;
  /* Poll type, currently can be “regular” or “quiz” */
  String type;
  /* True, if the poll allows multiple answers */
  bool allows_multiple_answers;
  /* Optional. Special entities that appear in the question. Currently, only custom emoji entities are allowed
   * in poll questions */
  optional<arrayof<MessageEntity>> question_entities;
  /* Optional. 0-based identifier of the correct answer option. Available only for polls in the quiz mode,
   * which are closed, or was sent (not forwarded) by the bot or to the private chat with the bot. */
  optional<Integer> correct_option_id;
  /* Optional. Text that is shown when a user chooses an incorrect answer or taps on the lamp icon in a
   * quiz-style poll, 0-200 characters */
  optional<String> explanation;
  /* Optional. Special entities like usernames, URLs, bot commands, etc. that appear in the explanation */
  optional<arrayof<MessageEntity>> explanation_entities;
  /* Optional. Amount of time in seconds the poll will be active after creation */
  optional<Integer> open_period;
  /* Optional. Point in time (Unix timestamp) when the poll will be automatically closed */
  optional<Integer> close_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("question", true)
        .case_("options", true)
        .case_("total_voter_count", true)
        .case_("is_closed", true)
        .case_("is_anonymous", true)
        .case_("type", true)
        .case_("allows_multiple_answers", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
