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
  /* Optional. The total number of gifts of this type that can be sent by all users; for limited gifts only */
  optional<Integer> total_count;
  /* Optional. The number of remaining gifts of this type that can be sent by all users; for limited gifts
   * only */
  optional<Integer> remaining_count;
  /* Optional. The total number of gifts of this type that can be sent by the bot; for limited gifts only */
  optional<Integer> personal_total_count;
  /* Optional. The number of remaining gifts of this type that can be sent by the bot; for limited gifts only
   */
  optional<Integer> personal_remaining_count;
  /* Optional. Background of the gift */
  box<GiftBackground> background;
  /* Optional. The total number of different unique gifts that can be obtained by upgrading the gift */
  optional<Integer> unique_gift_variant_count;
  /* Optional. Information about the chat that published the gift */
  box<Chat> publisher_chat;
  /* Optional. True, if the gift can only be purchased by Telegram Premium subscribers */
  optional<True> is_premium;
  /* Optional. True, if the gift can be used (after being upgraded) to customize a user's appearance */
  optional<True> has_colors;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("sticker", true)
        .case_("star_count", true)
        .or_default(false);
  }

  bool operator==(const Gift&) const;
  std::strong_ordering operator<=>(const Gift&) const;
};

}  // namespace tgbm::api
