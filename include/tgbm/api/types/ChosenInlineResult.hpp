#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Represents a result of an inline query that was chosen by the user and sent to their chat partner.*/
struct ChosenInlineResult {
  /* The unique identifier for the result that was chosen */
  String result_id;
  /* The user that chose the result */
  box<User> from;
  /* The query that was used to obtain the result */
  String query;
  /* Optional. Sender location, only for bots that require user location */
  box<Location> location;
  /* Optional. Identifier of the sent inline message. Available only if there is an inline keyboard attached
   * to the message. Will be also received in callback queries and can be used to edit the message. */
  optional<String> inline_message_id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("result_id", true)
        .case_("from", true)
        .case_("query", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
