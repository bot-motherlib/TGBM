#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes a unique gift that was upgraded from a regular gift.*/
struct UniqueGift {
  /* Identifier of the regular gift from which the gift was upgraded */
  String gift_id;
  /* Human-readable name of the regular gift from which this unique gift was upgraded */
  String base_name;
  /* Unique name of the gift. This name can be used in https://t.me/nft/... links and story areas */
  String name;
  /* Unique number of the upgraded gift among gifts upgraded from the same regular gift */
  Integer number;
  /* Model of the gift */
  box<UniqueGiftModel> model;
  /* Symbol of the gift */
  box<UniqueGiftSymbol> symbol;
  /* Backdrop of the gift */
  box<UniqueGiftBackdrop> backdrop;
  /* Optional. The color scheme that can be used by the gift's owner for the chat's name, replies to messages
   * and link previews; for business account gifts and gifts that are currently on sale only */
  box<UniqueGiftColors> colors;
  /* Optional. Information about the chat that published the gift */
  box<Chat> publisher_chat;
  /* Optional. True, if the original regular gift was exclusively purchaseable by Telegram Premium subscribers
   */
  optional<True> is_premium;
  /* Optional. True, if the gift was used to craft another gift and isn't available anymore */
  optional<True> is_burned;
  /* Optional. True, if the gift is assigned from the TON blockchain and can't be resold or transferred in
   * Telegram */
  optional<True> is_from_blockchain;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("gift_id", true)
        .case_("base_name", true)
        .case_("name", true)
        .case_("number", true)
        .case_("model", true)
        .case_("symbol", true)
        .case_("backdrop", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
