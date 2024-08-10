#ifndef TGBOT_GENERALFORUMTOPICUNHIDDEN_H
#define TGBOT_GENERALFORUMTOPICUNHIDDEN_H

#include <memory>

namespace tgbm {

/**
 * @brief This object represents a service message about General forum topic unhidden in the chat.
 *
 * Currently holds no information.
 *
 * @ingroup types
 */
class GeneralForumTopicUnhidden {
 public:
  typedef std::shared_ptr<GeneralForumTopicUnhidden> Ptr;
};
}  // namespace tgbm

#endif  // TGBOT_GENERALFORUMTOPICUNHIDDEN_H
