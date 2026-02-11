#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/UserProfileAudios.hpp>

namespace tgbm::api {

struct get_user_profile_audios_request {
  /* Unique identifier of the target user */
  Integer user_id;
  /* Sequential number of the first audio to be returned. By default, all audios are returned. */
  optional<Integer> offset;
  /* Limits the number of audios to be retrieved. Values between 1-100 are accepted. Defaults to 100. */
  optional<Integer> limit;

  using return_type = UserProfileAudios;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getUserProfileAudios";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    if (offset)
      body.arg("offset", *offset);
    if (limit)
      body.arg("limit", *limit);
  }
};

}  // namespace tgbm::api
