#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Contains information about the start page settings of a Telegram Business account.*/
struct BusinessIntro {
  /* Optional. Title text of the business intro */
  optional<String> title;
  /* Optional. Message text of the business intro */
  optional<String> message;
  /* Optional. Sticker of the business intro */
  box<Sticker> sticker;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
