#pragma once

#include "tgbm/types/InputMedia.h"

#include <memory>

namespace tgbm {

/**
 * @brief Represents a photo to be sent.
 *
 * @ingroup types
 */
class InputMediaPhoto : public InputMedia {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<InputMediaPhoto> Ptr;

  InputMediaPhoto() {
    this->type = TYPE;
  }

  /**
   * @brief Optional. Pass True if the photo needs to be covered with a spoiler animation
   */
  bool hasSpoiler;
};
}  // namespace tgbm
