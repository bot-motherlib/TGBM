#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a game. Use BotFather to create and edit games, their short names will act as unique
 * identifiers.*/
struct Game {
  /* Title of the game */
  String title;
  /* Description of the game */
  String description;
  /* Photo that will be displayed in the game message in chats. */
  arrayof<PhotoSize> photo;
  /* Optional. Brief description of the game or high scores included in the game message. Can be automatically
   * edited to include current high scores for the game when the bot calls setGameScore, or manually edited
   * using editMessageText. 0-4096 characters. */
  optional<String> text;
  /* Optional. Special entities that appear in text, such as usernames, URLs, bot commands, etc. */
  optional<arrayof<MessageEntity>> text_entities;
  /* Optional. Animation that will be displayed in the game message in chats. Upload via BotFather */
  box<Animation> animation;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("title", true)
        .case_("description", true)
        .case_("photo", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
