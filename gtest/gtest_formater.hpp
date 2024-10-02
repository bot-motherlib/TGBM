#pragma once

#include <iterator>
#include <tgbm/tools/formatters.hpp>
#include <fmt/ranges.h>

template <typename T>
concept formattable = fmt::is_formattable<T>::value;

namespace std {

template <formattable T>
void PrintTo(const T& t, std::ostream* os) {
  fmt::format_to(std::ostreambuf_iterator(*os), "{}", t);
}
}  // namespace std
