#pragma once

#include "tgbm/types/User.h"

#include <memory>
#include <vector>

namespace tgbm {

/**
 * @brief This object represents a service message about new members invited to a video chat.
 *
 * @ingroup types
 */
class VideoChatParticipantsInvited {
 public:
  typedef std::shared_ptr<VideoChatParticipantsInvited> Ptr;

  /**
   * @brief New members that were invited to the video chat
   */
  std::vector<User::Ptr> users;
};
}  // namespace tgbm
