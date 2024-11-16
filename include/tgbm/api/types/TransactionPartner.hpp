#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/TransactionPartnerUser.hpp>
#include <tgbm/api/types/TransactionPartnerFragment.hpp>
#include <tgbm/api/types/TransactionPartnerTelegramAds.hpp>
#include <tgbm/api/types/TransactionPartnerTelegramApi.hpp>
#include <tgbm/api/types/TransactionPartnerOther.hpp>

namespace tgbm::api {

/*This object describes the source of a transaction, or its recipient for outgoing transactions. Currently, it
 * can be one of*/
struct TransactionPartner {
  oneof<TransactionPartnerUser, TransactionPartnerFragment, TransactionPartnerTelegramAds,
        TransactionPartnerTelegramApi, TransactionPartnerOther>
      data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_user,
    k_fragment,
    k_telegramads,
    k_telegramapi,
    k_other,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  TransactionPartnerUser* get_user() noexcept {
    return data.get_if<TransactionPartnerUser>();
  }
  const TransactionPartnerUser* get_user() const noexcept {
    return data.get_if<TransactionPartnerUser>();
  }
  TransactionPartnerFragment* get_fragment() noexcept {
    return data.get_if<TransactionPartnerFragment>();
  }
  const TransactionPartnerFragment* get_fragment() const noexcept {
    return data.get_if<TransactionPartnerFragment>();
  }
  TransactionPartnerTelegramAds* get_telegramads() noexcept {
    return data.get_if<TransactionPartnerTelegramAds>();
  }
  const TransactionPartnerTelegramAds* get_telegramads() const noexcept {
    return data.get_if<TransactionPartnerTelegramAds>();
  }
  TransactionPartnerTelegramApi* get_telegramapi() noexcept {
    return data.get_if<TransactionPartnerTelegramApi>();
  }
  const TransactionPartnerTelegramApi* get_telegramapi() const noexcept {
    return data.get_if<TransactionPartnerTelegramApi>();
  }
  TransactionPartnerOther* get_other() noexcept {
    return data.get_if<TransactionPartnerOther>();
  }
  const TransactionPartnerOther* get_other() const noexcept {
    return data.get_if<TransactionPartnerOther>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("user", type_e::k_user)
        .case_("fragment", type_e::k_fragment)
        .case_("telegram_ads", type_e::k_telegramads)
        .case_("telegram_api", type_e::k_telegramapi)
        .case_("other", type_e::k_other)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "user")
      return visitor.template operator()<TransactionPartnerUser>();
    if (val == "fragment")
      return visitor.template operator()<TransactionPartnerFragment>();
    if (val == "telegram_ads")
      return visitor.template operator()<TransactionPartnerTelegramAds>();
    if (val == "telegram_api")
      return visitor.template operator()<TransactionPartnerTelegramApi>();
    if (val == "other")
      return visitor.template operator()<TransactionPartnerOther>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum TransactionPartner::type_e;
    switch (type()) {
      case k_user:
        return "user";
      case k_fragment:
        return "fragment";
      case k_telegramads:
        return "telegram_ads";
      case k_telegramapi:
        return "telegram_api";
      case k_other:
        return "other";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
