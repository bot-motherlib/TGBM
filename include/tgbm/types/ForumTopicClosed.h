#pragma once

#include <memory>

namespace tgbm {

/**
 * @brief This object represents a service message about a forum topic closed in the chat.
 *
 * Currently holds no information.
 *
 * @ingroup types
 */
class ForumTopicClosed {
 public:
  typedef std::shared_ptr<ForumTopicClosed> Ptr;
};
}  // namespace tgbm
