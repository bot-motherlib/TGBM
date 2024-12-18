#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes an inline message to be sent by a user of a Mini App.*/
struct PreparedInlineMessage {
  /* Unique identifier of the prepared message */
  String id;
  /* Expiration date of the prepared message, in Unix time. Expired prepared messages can no longer be used */
  Integer expiration_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("id", true).case_("expiration_date", true).or_default(false);
  }
};

}  // namespace tgbm::api
