#pragma once

#include "tgbm/types/BotCommandScope.h"

#include <memory>

namespace tgbm {

/**
 * @brief Represents the default scope of bot commands.
 * Default commands are used if no commands with a narrower scope are specified for the user.
 *
 * @ingroup types
 */
class BotCommandScopeDefault : public BotCommandScope {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<BotCommandScopeDefault> Ptr;

  BotCommandScopeDefault() {
    this->type = TYPE;
  }
};
}  // namespace tgbm
