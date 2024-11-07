#pragma once

#include "all_fwd.hpp"
#include "ChatBoostSourcePremium.hpp"
#include "ChatBoostSourceGiftCode.hpp"
#include "ChatBoostSourceGiveaway.hpp"

namespace tgbm::api {

/*This object describes the source of a chat boost. It can be one of*/
struct ChatBoostSource {
  oneof<ChatBoostSourcePremium, ChatBoostSourceGiftCode, ChatBoostSourceGiveaway> data;
  static constexpr std::string_view discriminator = "source";
  enum struct type_e {
    k_premium,
    k_giftcode,
    k_giveaway,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  ChatBoostSourcePremium* get_premium() noexcept {
    return data.get_if<ChatBoostSourcePremium>();
  }
  const ChatBoostSourcePremium* get_premium() const noexcept {
    return data.get_if<ChatBoostSourcePremium>();
  }
  ChatBoostSourceGiftCode* get_giftcode() noexcept {
    return data.get_if<ChatBoostSourceGiftCode>();
  }
  const ChatBoostSourceGiftCode* get_giftcode() const noexcept {
    return data.get_if<ChatBoostSourceGiftCode>();
  }
  ChatBoostSourceGiveaway* get_giveaway() noexcept {
    return data.get_if<ChatBoostSourceGiveaway>();
  }
  const ChatBoostSourceGiveaway* get_giveaway() const noexcept {
    return data.get_if<ChatBoostSourceGiveaway>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return utils::string_switch<type_e>(val)
        .case_("premium", type_e::k_premium)
        .case_("gift_code", type_e::k_giftcode)
        .case_("giveaway", type_e::k_giveaway)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "premium")
      return visitor.template operator()<ChatBoostSourcePremium>();
    if (val == "gift_code")
      return visitor.template operator()<ChatBoostSourceGiftCode>();
    if (val == "giveaway")
      return visitor.template operator()<ChatBoostSourceGiveaway>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum ChatBoostSource::type_e;
    switch (type()) {
      case k_premium:
        return "premium";
      case k_giftcode:
        return "gift_code";
      case k_giveaway:
        return "giveaway";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
