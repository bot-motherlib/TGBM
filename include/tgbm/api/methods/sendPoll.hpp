#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InputPollOption.hpp>
#include <tgbm/api/types/Message.hpp>
#include <tgbm/api/types/MessageEntity.hpp>
#include <tgbm/api/types/ReplyParameters.hpp>

namespace tgbm::api {

struct send_poll_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername).
   * Polls can't be sent to channel direct messages chats. */
  int_or_str chat_id;
  /* Poll question, 1-300 characters */
  String question;
  /* A JSON-serialized list of 2-12 answer options */
  arrayof<InputPollOption> options;
  /* Unique identifier of the business connection on behalf of which the message will be sent */
  optional<String> business_connection_id;
  /* Unique identifier for the target message thread (topic) of a forum; for forum supergroups and private
   * chats of bots with forum topic mode enabled only */
  optional<Integer> message_thread_id;
  /* Mode for parsing entities in the question. See formatting options for more details. Currently, only
   * custom emoji entities are allowed */
  optional<String> question_parse_mode;
  /* A JSON-serialized list of special entities that appear in the poll question. It can be specified instead
   * of question_parse_mode */
  optional<arrayof<MessageEntity>> question_entities;
  /* True, if the poll needs to be anonymous, defaults to True */
  optional<bool> is_anonymous;
  /* Poll type, “quiz” or “regular”, defaults to “regular” */
  optional<String> type;
  /* Pass True, if the poll allows multiple answers, defaults to False */
  optional<bool> allows_multiple_answers;
  /* Pass True, if the poll allows to change chosen answer options, defaults to False for quizzes and to True
   * for regular polls */
  optional<bool> allows_revoting;
  /* Pass True, if the poll options must be shown in random order */
  optional<bool> shuffle_options;
  /* Pass True, if answer options can be added to the poll after creation; not supported for anonymous polls
   * and quizzes */
  optional<bool> allow_adding_options;
  /* Pass True, if poll results must be shown only after the poll closes */
  optional<bool> hide_results_until_closes;
  /* A JSON-serialized list of monotonically increasing 0-based identifiers of the correct answer options,
   * required for polls in quiz mode */
  optional<arrayof<Integer>> correct_option_ids;
  /* Text that is shown when a user chooses an incorrect answer or taps on the lamp icon in a quiz-style poll,
   * 0-200 characters with at most 2 line feeds after entities parsing */
  optional<String> explanation;
  /* Mode for parsing entities in the explanation. See formatting options for more details. */
  optional<String> explanation_parse_mode;
  /* A JSON-serialized list of special entities that appear in the poll explanation. It can be specified
   * instead of explanation_parse_mode */
  optional<arrayof<MessageEntity>> explanation_entities;
  /* Amount of time in seconds the poll will be active after creation, 5-2628000. Can't be used together with
   * close_date. */
  optional<Integer> open_period;
  /* Point in time (Unix timestamp) when the poll will be automatically closed. Must be at least 5 and no more
   * than 2628000 seconds in the future. Can't be used together with open_period. */
  optional<Integer> close_date;
  /* Pass True if the poll needs to be immediately closed. This can be useful for poll preview. */
  optional<bool> is_closed;
  /* Description of the poll to be sent, 0-1024 characters after entities parsing */
  optional<String> description;
  /* Mode for parsing entities in the poll description. See formatting options for more details. */
  optional<String> description_parse_mode;
  /* A JSON-serialized list of special entities that appear in the poll description, which can be specified
   * instead of description_parse_mode */
  optional<arrayof<MessageEntity>> description_entities;
  /* Sends the message silently. Users will receive a notification with no sound. */
  optional<bool> disable_notification;
  /* Protects the contents of the sent message from forwarding and saving */
  optional<bool> protect_content;
  /* Pass True to allow up to 1000 messages per second, ignoring broadcasting limits for a fee of 0.1 Telegram
   * Stars per message. The relevant Stars will be withdrawn from the bot's balance */
  optional<bool> allow_paid_broadcast;
  /* Unique identifier of the message effect to be added to the message; for private chats only */
  optional<String> message_effect_id;
  /* Description of the message to reply to */
  box<ReplyParameters> reply_parameters;
  /* Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard,
   * instructions to remove a reply keyboard or to force a reply from the user */
  optional<reply_markup_t> reply_markup;

  using return_type = Message;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "sendPoll";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    body.arg("question", question);
    if (question_parse_mode)
      body.arg("question_parse_mode", *question_parse_mode);
    if (question_entities)
      body.arg("question_entities", *question_entities);
    body.arg("options", options);
    if (is_anonymous)
      body.arg("is_anonymous", *is_anonymous);
    if (type)
      body.arg("type", *type);
    if (allows_multiple_answers)
      body.arg("allows_multiple_answers", *allows_multiple_answers);
    if (allows_revoting)
      body.arg("allows_revoting", *allows_revoting);
    if (shuffle_options)
      body.arg("shuffle_options", *shuffle_options);
    if (allow_adding_options)
      body.arg("allow_adding_options", *allow_adding_options);
    if (hide_results_until_closes)
      body.arg("hide_results_until_closes", *hide_results_until_closes);
    if (correct_option_ids)
      body.arg("correct_option_ids", *correct_option_ids);
    if (explanation)
      body.arg("explanation", *explanation);
    if (explanation_parse_mode)
      body.arg("explanation_parse_mode", *explanation_parse_mode);
    if (explanation_entities)
      body.arg("explanation_entities", *explanation_entities);
    if (open_period)
      body.arg("open_period", *open_period);
    if (close_date)
      body.arg("close_date", *close_date);
    if (is_closed)
      body.arg("is_closed", *is_closed);
    if (description)
      body.arg("description", *description);
    if (description_parse_mode)
      body.arg("description_parse_mode", *description_parse_mode);
    if (description_entities)
      body.arg("description_entities", *description_entities);
    if (disable_notification)
      body.arg("disable_notification", *disable_notification);
    if (protect_content)
      body.arg("protect_content", *protect_content);
    if (allow_paid_broadcast)
      body.arg("allow_paid_broadcast", *allow_paid_broadcast);
    if (message_effect_id)
      body.arg("message_effect_id", *message_effect_id);
    if (reply_parameters)
      body.arg("reply_parameters", *reply_parameters);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
