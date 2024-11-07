#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/File.hpp"

namespace tgbm::api {

struct get_file_request {
  /* File identifier to get information about */
  String file_id;

  using return_type = File;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getFile";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("file_id", file_id);
  }
};

}  // namespace tgbm::api
