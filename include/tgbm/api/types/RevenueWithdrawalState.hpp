#pragma once

#include "all_fwd.hpp"
#include "RevenueWithdrawalStatePending.hpp"
#include "RevenueWithdrawalStateSucceeded.hpp"
#include "RevenueWithdrawalStateFailed.hpp"

namespace tgbm::api {

/*This object describes the state of a revenue withdrawal operation. Currently, it can be one of*/
struct RevenueWithdrawalState {
  oneof<RevenueWithdrawalStatePending, RevenueWithdrawalStateSucceeded, RevenueWithdrawalStateFailed> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_pending,
    k_succeeded,
    k_failed,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  RevenueWithdrawalStatePending* get_pending() noexcept {
    return data.get_if<RevenueWithdrawalStatePending>();
  }
  const RevenueWithdrawalStatePending* get_pending() const noexcept {
    return data.get_if<RevenueWithdrawalStatePending>();
  }
  RevenueWithdrawalStateSucceeded* get_succeeded() noexcept {
    return data.get_if<RevenueWithdrawalStateSucceeded>();
  }
  const RevenueWithdrawalStateSucceeded* get_succeeded() const noexcept {
    return data.get_if<RevenueWithdrawalStateSucceeded>();
  }
  RevenueWithdrawalStateFailed* get_failed() noexcept {
    return data.get_if<RevenueWithdrawalStateFailed>();
  }
  const RevenueWithdrawalStateFailed* get_failed() const noexcept {
    return data.get_if<RevenueWithdrawalStateFailed>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return utils::string_switch<type_e>(val)
        .case_("pending", type_e::k_pending)
        .case_("succeeded", type_e::k_succeeded)
        .case_("failed", type_e::k_failed)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "pending")
      return visitor.template operator()<RevenueWithdrawalStatePending>();
    if (val == "succeeded")
      return visitor.template operator()<RevenueWithdrawalStateSucceeded>();
    if (val == "failed")
      return visitor.template operator()<RevenueWithdrawalStateFailed>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum RevenueWithdrawalState::type_e;
    switch (type()) {
      case k_pending:
        return "pending";
      case k_succeeded:
        return "succeeded";
      case k_failed:
        return "failed";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
