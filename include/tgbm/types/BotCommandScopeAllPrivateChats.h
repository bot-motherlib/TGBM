#pragma once

#include "tgbm/types/BotCommandScope.h"

#include <memory>

namespace tgbm {

/**
 * @brief Represents the scope of bot commands, covering all private chats.
 *
 * @ingroup types
 */
class BotCommandScopeAllPrivateChats : public BotCommandScope {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<BotCommandScopeAllPrivateChats> Ptr;

  BotCommandScopeAllPrivateChats() {
    this->type = TYPE;
  }
};
}  // namespace tgbm
