#pragma once

#include "tgbm/types/GenericReply.h"
#include "tgbm/types/InlineKeyboardButton.h"

#include <vector>
#include <memory>

namespace tgbm {

/**
 * @brief This object represents an [inline
 * keyboard](https://core.telegram.org/bots/features#inline-keyboards) that appears right next to the message
 * it belongs to.
 *
 * @ingroup types
 */
// TODO 7.3 api version etc
class InlineKeyboardMarkup : public GenericReply {
 public:
  typedef std::shared_ptr<InlineKeyboardMarkup> Ptr;

  /**
   * @brief Array of button rows, each represented by an Array of InlineKeyboardButton objects
   */
  std::vector<std::vector<InlineKeyboardButton::Ptr>> inlineKeyboard;
};
}  // namespace tgbm
