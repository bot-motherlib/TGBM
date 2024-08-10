#pragma once

#include "tgbm/types/BotCommandScope.h"

#include <memory>

namespace tgbm {

/**
 * @brief Represents the scope of bot commands, covering all group and supergroup chat administrators.
 *
 * @ingroup types
 */
class BotCommandScopeAllChatAdministrators : public BotCommandScope {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<BotCommandScopeAllChatAdministrators> Ptr;

  BotCommandScopeAllChatAdministrators() {
    this->type = TYPE;
  }
};
}  // namespace tgbm
