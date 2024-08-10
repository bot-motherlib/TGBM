#pragma once

#include "tgbm/types/MenuButton.h"
#include "tgbm/types/WebAppInfo.h"

#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief Represents a menu button, which launches a Web App.
 *
 * @ingroup types
 */
class MenuButtonWebApp : public MenuButton {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<MenuButtonWebApp> Ptr;

  MenuButtonWebApp() {
    this->type = TYPE;
  }

  /**
   * @brief Text on the button
   */
  std::string text;

  /**
   * @brief Description of the Web App that will be launched when the user presses the button.
   * The Web App will be able to send an arbitrary message on behalf of the user using the method
   * Api::answerWebAppQuery.
   */
  WebAppInfo::Ptr webApp;
};
}  // namespace tgbm
