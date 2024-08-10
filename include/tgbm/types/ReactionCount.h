#ifndef TGBOT_REACTIONCOUNT_H
#define TGBOT_REACTIONCOUNT_H

#include "tgbm/types/ReactionType.h"

#include <cstdint>
#include <memory>

namespace tgbm {

/**
 * @brief Represents a reaction added to a message along with the number of times it was added.
 *
 * @ingroup types
 */
class ReactionCount {
 public:
  typedef std::shared_ptr<ReactionCount> Ptr;

  /**
   * @brief Type of the reaction
   */
  ReactionType::Ptr type;

  /**
   * @brief Number of times the reaction was added
   */
  std::int32_t totalCount;
};
}  // namespace tgbm

#endif  // TGBOT_REACTIONCOUNT_H
