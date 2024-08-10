#ifndef TGBOT_MENUBUTTONDEFAULT_H
#define TGBOT_MENUBUTTONDEFAULT_H

#include "tgbm/types/MenuButton.h"

#include <memory>

namespace tgbm {

/**
 * @brief Describes that no specific value for the menu button was set.
 *
 * @ingroup types
 */
class MenuButtonDefault : public MenuButton {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<MenuButtonDefault> Ptr;

  MenuButtonDefault() {
    this->type = TYPE;
  }
};
}  // namespace tgbm

#endif  // TGBOT_MENUBUTTONDEFAULT_H