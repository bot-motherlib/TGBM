#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a service message about a user allowing a bot to write messages after adding it to
 * the attachment menu, launching a Web App from a link, or accepting an explicit request from a Web App sent
 * by the method requestWriteAccess.*/
struct WriteAccessAllowed {
  /* Optional. Name of the Web App, if the access was granted when the Web App was launched from a link */
  optional<String> web_app_name;
  /* Optional. True, if the access was granted after the user accepted an explicit request from a Web App sent
   * by the method requestWriteAccess */
  optional<bool> from_request;
  /* Optional. True, if the access was granted when the bot was added to the attachment or side menu */
  optional<bool> from_attachment_menu;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
