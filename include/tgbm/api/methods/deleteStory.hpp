#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct delete_story_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Unique identifier of the story to delete */
  Integer story_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "deleteStory";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("story_id", story_id);
  }
};

}  // namespace tgbm::api
