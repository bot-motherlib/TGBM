#pragma once

#include <memory>

namespace tgbm {

/**
 * @brief This object represents a service message about a video chat started in the chat.
 * Currently holds no information.
 *
 * @ingroup types
 */
class VideoChatStarted {
 public:
  typedef std::shared_ptr<VideoChatStarted> Ptr;
};
}  // namespace tgbm
