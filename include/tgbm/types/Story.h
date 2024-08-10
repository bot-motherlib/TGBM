#pragma once

#include "tgbm/types/Chat.h"

#include <cstdint>
#include <memory>

namespace tgbm {

/**
 * @brief This object represents a story.
 *
 * @ingroup types
 */
class Story {
 public:
  typedef std::shared_ptr<Story> Ptr;

  /**
   * @brief Chat that posted the story
   */
  Chat::Ptr chat;

  /**
   * @brief Unique identifier for the story in the chat
   */
  std::int32_t id;
};
}  // namespace tgbm
