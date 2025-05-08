#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/OwnedGiftRegular.hpp>
#include <tgbm/api/types/OwnedGiftUnique.hpp>

namespace tgbm::api {

/*This object describes a gift received and owned by a user or a chat. Currently, it can be one of*/
struct OwnedGift {
  oneof<OwnedGiftRegular, OwnedGiftUnique> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_regular,
    k_unique,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  OwnedGiftRegular* get_regular() noexcept {
    return data.get_if<OwnedGiftRegular>();
  }
  const OwnedGiftRegular* get_regular() const noexcept {
    return data.get_if<OwnedGiftRegular>();
  }
  OwnedGiftUnique* get_unique() noexcept {
    return data.get_if<OwnedGiftUnique>();
  }
  const OwnedGiftUnique* get_unique() const noexcept {
    return data.get_if<OwnedGiftUnique>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("regular", type_e::k_regular)
        .case_("unique", type_e::k_unique)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "regular")
      return visitor.template operator()<OwnedGiftRegular>();
    if (val == "unique")
      return visitor.template operator()<OwnedGiftUnique>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum OwnedGift::type_e;
    switch (type()) {
      case k_regular:
        return "regular";
      case k_unique:
        return "unique";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
