#pragma once

#include <memory>

namespace tgbm {

/**
 * @brief A placeholder, currently holds no information.
 * Use BotFather to set up your game.
 *
 * @ingroup types
 */
class CallbackGame {
 public:
  typedef std::shared_ptr<CallbackGame> Ptr;
};
}  // namespace tgbm
