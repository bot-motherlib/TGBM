#pragma once

#include <compare>

namespace tgbm::api {

struct True {
  constexpr std::strong_ordering operator<=>(const True&) const = default;
};

}  // namespace tgbm::api
