#ifndef TGBOT_BOTNAME_H
#define TGBOT_BOTNAME_H

#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief This object represents the bot's name.
 *
 * @ingroup types
 */
class BotName {
 public:
  typedef std::shared_ptr<BotName> Ptr;

  /**
   * @brief The bot's name
   */
  std::string name;
};
}  // namespace tgbm

#endif  // TGBOT_BOTNAME_H
