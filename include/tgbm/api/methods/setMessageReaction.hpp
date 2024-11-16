#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/ReactionType.hpp>

namespace tgbm::api {

struct set_message_reaction_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Identifier of the target message. If the message belongs to a media group, the reaction is set to the
   * first non-deleted message in the group instead. */
  Integer message_id;
  /* A JSON-serialized list of reaction types to set on the message. Currently, as non-premium users, bots can
   * set up to one reaction per message. A custom emoji reaction can be used if it is either already present
   * on the message or explicitly allowed by chat administrators. Paid reactions can't be used by bots. */
  optional<arrayof<ReactionType>> reaction;
  /* Pass True to set the reaction with a big animation */
  optional<bool> is_big;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setMessageReaction";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("message_id", message_id);
    if (reaction)
      body.arg("reaction", *reaction);
    if (is_big)
      body.arg("is_big", *is_big);
  }
};

}  // namespace tgbm::api
