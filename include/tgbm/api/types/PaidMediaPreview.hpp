#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The paid media isn't available before the payment.*/
struct PaidMediaPreview {
  /* Optional. Media width as defined by the sender */
  optional<Integer> width;
  /* Optional. Media height as defined by the sender */
  optional<Integer> height;
  /* Optional. Duration of the media in seconds as defined by the sender */
  optional<Integer> duration;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
