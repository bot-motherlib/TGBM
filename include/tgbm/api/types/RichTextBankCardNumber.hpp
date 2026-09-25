#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A text with a bank card number.*/
struct RichTextBankCardNumber {
  /* The text */
  box<RichText> text;
  /* The bank card number */
  String bank_card_number;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("bank_card_number", true).or_default(false);
  }
};

}  // namespace tgbm::api
