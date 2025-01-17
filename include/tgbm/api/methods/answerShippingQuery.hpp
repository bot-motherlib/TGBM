#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/ShippingOption.hpp>

namespace tgbm::api {

struct answer_shipping_query_request {
  /* Unique identifier for the query to be answered */
  String shipping_query_id;
  /* Pass True if delivery to the specified address is possible and False if there are any problems (for
   * example, if delivery to the specified address is not possible) */
  bool ok;
  /* Required if ok is True. A JSON-serialized array of available shipping options. */
  optional<arrayof<ShippingOption>> shipping_options;
  /* Required if ok is False. Error message in human readable form that explains why it is impossible to
   * complete the order (e.g. “Sorry, delivery to your desired address is unavailable”). Telegram will display
   * this message to the user. */
  optional<String> error_message;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "answerShippingQuery";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("shipping_query_id", shipping_query_id);
    body.arg("ok", ok);
    if (shipping_options)
      body.arg("shipping_options", *shipping_options);
    if (error_message)
      body.arg("error_message", *error_message);
  }
};

}  // namespace tgbm::api
