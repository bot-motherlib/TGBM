#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct delete_webhook_request {
  /* Pass True to drop all pending updates */
  optional<bool> drop_pending_updates;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "deleteWebhook";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (drop_pending_updates)
      body.arg("drop_pending_updates", *drop_pending_updates);
  }
};

}  // namespace tgbm::api
