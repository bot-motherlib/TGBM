#pragma once

#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief This abstract class is base of all message contents.
 *
 * This object represents the content of a message to be sent as a result of an inline query.
 *
 * @ingroup types
 */
class InputMessageContent {
 public:
  typedef std::shared_ptr<InputMessageContent> Ptr;

  InputMessageContent() {
  }

  virtual ~InputMessageContent() {
  }

  /**
   * @brief Type of the content
   */
  std::string type;
};
}  // namespace tgbm
