#pragma once

#include <memory>

namespace tgbm {

/**
 * @brief This object represents a service message about a forum topic reopened in the chat.
 *
 * Currently holds no information.
 *
 * @ingroup types
 */
class ForumTopicReopened {
 public:
  typedef std::shared_ptr<ForumTopicReopened> Ptr;
};
}  // namespace tgbm
