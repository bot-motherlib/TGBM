#pragma once

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
