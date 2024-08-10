#pragma once

#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief This object represents the bot's description.
 *
 * @ingroup types
 */
class BotDescription {
 public:
  typedef std::shared_ptr<BotDescription> Ptr;

  /**
   * @brief The bot's description
   */
  std::string description;
};
}  // namespace tgbm
