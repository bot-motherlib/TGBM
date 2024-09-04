#pragma once

#include <compare>
#include <string>
#include <variant>
#include <vector>
#include <tgbm/tools/StringTools.h>
#include <tgbm/tools/box_union.hpp>
#include <tgbm/api/optional.hpp>
#include <tgbm/api/Integer.hpp>

namespace tgbm::api {

using String = std::string;
using Boolean = bool;
struct True {
  constexpr std::strong_ordering operator<=>(const True&) const = default;
};

template <typename T>
using arrayof = std::vector<T>;

template <typename T, typename... U>
using oneof = box_union<U...>;

// using FileOrStr = nothing_t;

using InrOrStr = std::variant<Integer, std::string>;

}  // namespace tgbm::api
