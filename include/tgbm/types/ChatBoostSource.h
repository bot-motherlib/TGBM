#pragma once

#include "tgbm/types/User.h"

#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief This object describes the source of a chat boost.
 *
 * It can be one of
 * - ChatBoostSourcePremium
 * - ChatBoostSourceGiftCode
 * - ChatBoostSourceGiveaway
 *
 * @ingroup types
 */
class ChatBoostSource {
 public:
  typedef std::shared_ptr<ChatBoostSource> Ptr;

  ChatBoostSource() {
  }

  virtual ~ChatBoostSource() {
  }

  /**
   * @brief Source of the boost
   */
  std::string source;

  /**
   * @brief User
   */
  User::Ptr user;
};
}  // namespace tgbm
