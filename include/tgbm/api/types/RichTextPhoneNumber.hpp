#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A text with a phone number.*/
struct RichTextPhoneNumber {
  /* The text */
  box<RichText> text;
  /* The phone number */
  String phone_number;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("phone_number", true).or_default(false);
  }
};

}  // namespace tgbm::api
