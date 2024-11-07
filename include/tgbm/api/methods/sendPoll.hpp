#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/Message.hpp"
#include "tgbm/api/types/MessageEntity.hpp"
#include "tgbm/api/types/InputPollOption.hpp"
#include "tgbm/api/types/ReplyParameters.hpp"

namespace tgbm::api {

struct send_poll_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Poll question, 1-300 characters */
  String question;
  /* A JSON-serialized list of 2-10 answer options */
  arrayof<InputPollOption> options;
  /* Unique identifier of the business connection on behalf of which the message will be sent */
  optional<String> business_connection_id;
  /* Unique identifier for the target message thread (topic) of the forum; for forum supergroups only */
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
  /* True, if the poll allows multiple answers, ignored for polls in quiz mode, defaults to False */
  optional<bool> allows_multiple_answers;
  /* 0-based identifier of the correct answer option, required for polls in quiz mode */
  optional<Integer> correct_option_id;
  /* Text that is shown when a user chooses an incorrect answer or taps on the lamp icon in a quiz-style poll,
   * 0-200 characters with at most 2 line feeds after entities parsing */
  optional<String> explanation;
  /* Mode for parsing entities in the explanation. See formatting options for more details. */
  optional<String> explanation_parse_mode;
  /* A JSON-serialized list of special entities that appear in the poll explanation. It can be specified
   * instead of explanation_parse_mode */
  optional<arrayof<MessageEntity>> explanation_entities;
  /* Amount of time in seconds the poll will be active after creation, 5-600. Can't be used together with
   * close_date. */
  optional<Integer> open_period;
  /* Point in time (Unix timestamp) when the poll will be automatically closed. Must be at least 5 and no more
   * than 600 seconds in the future. Can't be used together with open_period. */
  optional<Integer> close_date;
  /* Pass True if the poll needs to be immediately closed. This can be useful for poll preview. */
  optional<bool> is_closed;
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
    if (correct_option_id)
      body.arg("correct_option_id", *correct_option_id);
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
