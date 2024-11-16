#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes data sent from a Web App to the bot.*/
struct WebAppData {
  /* The data. Be aware that a bad client can send arbitrary data in this field. */
  String data;
  /* Text of the web_app keyboard button from which the Web App was opened. Be aware that a bad client can
   * send arbitrary data in this field. */
  String button_text;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("data", true).case_("button_text", true).or_default(false);
  }
};

}  // namespace tgbm::api
