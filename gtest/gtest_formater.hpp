#pragma once

#include <iterator>

#include <fmt/ranges.h>

#include "tgbm/utils/formatters.hpp"

template <typename T>
concept formattable = fmt::is_formattable<T>::value;

namespace std {

template <formattable T>
void PrintTo(const T& t, std::ostream* os) {
  fmt::format_to(std::ostreambuf_iterator(*os), "{}", t);
}
}  // namespace std
