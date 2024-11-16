#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/PassportElementError.hpp>

namespace tgbm::api {

struct set_passport_data_errors_request {
  /* User identifier */
  Integer user_id;
  /* A JSON-serialized array describing the errors */
  arrayof<PassportElementError> errors;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setPassportDataErrors";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("errors", errors);
  }
};

}  // namespace tgbm::api
