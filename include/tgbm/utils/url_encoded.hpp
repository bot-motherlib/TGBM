#pragma once

#include <algorithm>
#include <cstddef>
#include <string>

#include <fmt/format.h>

namespace tgbm {

template <typename O>
O url_encode(char c, O out) {
  static constexpr auto is_legit = [](char c) {
    switch (c) {
      case 'A' ... 'Z':
      case 'a' ... 'z':
      case '0' ... '9':
      case '_':
      case '.':
      case '-':
      case '~':
      case ':':
        return true;
      default:
        return false;
    }
  };
  if (is_legit(c)) {
    *out = c;
    ++out;
  } else
    out = fmt::format_to(out, "%{:02X}", (unsigned)(unsigned char)c);
  return out;
}

std::string generate_multipart_boundary(size_t length);

// used for fmt formatting
struct url_encoded {
  std::string_view str;
};

[[nodiscard]] static constexpr bool is_lowercase(std::string_view s) noexcept {
  auto is_uppercase_char = [](char c) { return c >= 'A' && c <= 'Z'; };
  return std::none_of(s.begin(), s.end(), is_uppercase_char);
}

}  // namespace tgbm

namespace fmt {

template <>
struct formatter<::tgbm::url_encoded> {
  static constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.end();
  }

  static auto format(::tgbm::url_encoded s, auto& ctx) -> decltype(ctx.out()) {
    auto it = ctx.out();
    for (const char& c : s.str)
      it = ::tgbm::url_encode(c, it);
    return it;
  }
};

}  // namespace fmt
