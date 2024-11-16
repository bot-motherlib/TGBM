#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/ReactionTypeEmoji.hpp>
#include <tgbm/api/types/ReactionTypeCustomEmoji.hpp>
#include <tgbm/api/types/ReactionTypePaid.hpp>

namespace tgbm::api {

/*This object describes the type of a reaction. Currently, it can be one of*/
struct ReactionType {
  oneof<ReactionTypeEmoji, ReactionTypeCustomEmoji, ReactionTypePaid> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_emoji,
    k_customemoji,
    k_paid,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  ReactionTypeEmoji* get_emoji() noexcept {
    return data.get_if<ReactionTypeEmoji>();
  }
  const ReactionTypeEmoji* get_emoji() const noexcept {
    return data.get_if<ReactionTypeEmoji>();
  }
  ReactionTypeCustomEmoji* get_customemoji() noexcept {
    return data.get_if<ReactionTypeCustomEmoji>();
  }
  const ReactionTypeCustomEmoji* get_customemoji() const noexcept {
    return data.get_if<ReactionTypeCustomEmoji>();
  }
  ReactionTypePaid* get_paid() noexcept {
    return data.get_if<ReactionTypePaid>();
  }
  const ReactionTypePaid* get_paid() const noexcept {
    return data.get_if<ReactionTypePaid>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("emoji", type_e::k_emoji)
        .case_("custom_emoji", type_e::k_customemoji)
        .case_("paid", type_e::k_paid)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "emoji")
      return visitor.template operator()<ReactionTypeEmoji>();
    if (val == "custom_emoji")
      return visitor.template operator()<ReactionTypeCustomEmoji>();
    if (val == "paid")
      return visitor.template operator()<ReactionTypePaid>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum ReactionType::type_e;
    switch (type()) {
      case k_emoji:
        return "emoji";
      case k_customemoji:
        return "custom_emoji";
      case k_paid:
        return "paid";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
