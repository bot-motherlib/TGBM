#pragma once

#include <memory>

namespace tgbm {

/**
 * @brief This abstract class is base of all keyboard related events.
 * @ingroup types
 */
class GenericReply {
 public:
  typedef std::shared_ptr<GenericReply> Ptr;

  virtual ~GenericReply() {
  }
};

}  // namespace tgbm
