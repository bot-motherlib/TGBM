#pragma once

#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief This object represents the bot's short description.
 *
 * @ingroup types
 */
class BotShortDescription {
 public:
  typedef std::shared_ptr<BotShortDescription> Ptr;

  /**
   * @brief The bot's short description
   */
  std::string shortDescription;
};
}  // namespace tgbm
