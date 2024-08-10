#pragma once

#include "tgbm/types/Chat.h"
#include "tgbm/types/User.h"

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

namespace tgbm {

/**
 * @brief This object represents an answer of a user in a non-anonymous poll.
 *
 * @ingroup types
 */
class PollAnswer {
 public:
  typedef std::shared_ptr<PollAnswer> Ptr;

  /**
   * @brief Unique poll identifier
   */
  std::string pollId;

  /**
   * @brief Optional. The chat that changed the answer to the poll, if the voter is anonymous
   */
  Chat::Ptr voterChat;

  /**
   * @brief Optional. The user that changed the answer to the poll, if the voter isn't anonymous
   */
  User::Ptr user;

  /**
   * @brief 0-based identifiers of chosen answer options.
   *
   * May be empty if the vote was retracted.
   */
  std::vector<std::int32_t> optionIds;
};
}  // namespace tgbm
