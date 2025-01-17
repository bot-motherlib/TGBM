#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a gift that can be sent by the bot.*/
struct Gift {
  /* Unique identifier of the gift */
  String id;
  /* The sticker that represents the gift */
  box<Sticker> sticker;
  /* The number of Telegram Stars that must be paid to send the sticker */
  Integer star_count;
  /* Optional. The number of Telegram Stars that must be paid to upgrade the gift to a unique one */
  optional<Integer> upgrade_star_count;
  /* Optional. The total number of the gifts of this type that can be sent; for limited gifts only */
  optional<Integer> total_count;
  /* Optional. The number of remaining gifts of this type that can be sent; for limited gifts only */
  optional<Integer> remaining_count;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("sticker", true)
        .case_("star_count", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
