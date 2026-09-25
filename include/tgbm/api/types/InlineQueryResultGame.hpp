#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents a Game.*/
struct InlineQueryResultGame {
  /* Unique identifier for this result, 1-64 bytes */
  String id;
  /* Short name of the game */
  String game_short_name;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("id", true).case_("game_short_name", true).or_default(false);
  }
};

}  // namespace tgbm::api
