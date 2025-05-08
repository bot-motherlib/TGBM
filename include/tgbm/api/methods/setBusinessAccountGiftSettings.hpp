#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/AcceptedGiftTypes.hpp>

namespace tgbm::api {

struct set_business_account_gift_settings_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Pass True, if a button for sending a gift to the user or by the business account must always be shown in
   * the input field */
  bool show_gift_button;
  /* Types of gifts accepted by the business account */
  box<AcceptedGiftTypes> accepted_gift_types;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setBusinessAccountGiftSettings";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("show_gift_button", show_gift_button);
    body.arg("accepted_gift_types", accepted_gift_types);
  }
};

}  // namespace tgbm::api
