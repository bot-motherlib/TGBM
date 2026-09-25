#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents a menu button, which launches a Web App.*/
struct MenuButtonWebApp {
  /* Text on the button */
  String text;
  /* Description of the Web App that will be launched when the user presses the button. The Web App will be
   * able to send an arbitrary message on behalf of the user using the method answerWebAppQuery.
   * Alternatively, a t.me link to a Web App of the bot can be specified in the object instead of the Web
   * App's URL, in which case the Web App will be opened as if the user pressed the link. */
  box<WebAppInfo> web_app;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("web_app", true).or_default(false);
  }
};

}  // namespace tgbm::api
