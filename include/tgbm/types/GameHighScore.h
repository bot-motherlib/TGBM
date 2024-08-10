#ifndef TGBOT_GAMEHIGHSCORE_H
#define TGBOT_GAMEHIGHSCORE_H

#include "tgbm/types/PhotoSize.h"
#include "tgbm/types/User.h"

#include <cstdint>
#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief This object represents one row of the high scores table for a game.
 *
 * @ingroup types
 */
class GameHighScore {
 public:
  typedef std::shared_ptr<GameHighScore> Ptr;

  /**
   * @brief Position in high score table for the game.
   */
  std::string position;

  /**
   * @brief User.
   */
  User::Ptr user;

  /**
   * @brief Score.
   */
  std::int32_t score;
};
}  // namespace tgbm

#endif  // TGBOT_GAMEHIGHSCORE_H
