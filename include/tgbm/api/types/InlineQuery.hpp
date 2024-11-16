#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents an incoming inline query. When the user sends an empty query, your bot could return
 * some default or trending results.*/
struct InlineQuery {
  /* Unique identifier for this query */
  String id;
  /* Sender */
  box<User> from;
  /* Text of the query (up to 256 characters) */
  String query;
  /* Offset of the results to be returned, can be controlled by the bot */
  String offset;
  /* Optional. Type of the chat from which the inline query was sent. Can be either “sender” for a private
   * chat with the inline query sender, “private”, “group”, “supergroup”, or “channel”. The chat type should
   * be always known for requests sent from official clients and most third-party clients, unless the request
   * was sent from a secret chat */
  optional<String> chat_type;
  /* Optional. Sender location, only for bots that request user location */
  box<Location> location;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("from", true)
        .case_("query", true)
        .case_("offset", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
