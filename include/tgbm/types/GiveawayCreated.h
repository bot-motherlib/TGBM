#pragma once

#include <memory>

namespace tgbm {

/**
 * @brief This object represents a service message about the creation of a scheduled giveaway.
 *
 * Currently holds no information.
 *
 * @ingroup types
 */
class GiveawayCreated {
 public:
  typedef std::shared_ptr<GiveawayCreated> Ptr;
};
}  // namespace tgbm
