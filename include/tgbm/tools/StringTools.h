#pragma once

#include "tgbm/export.h"

#include <cstddef>
#include <vector>
#include <string>

#include <fmt/format.h>

/**
 * @ingroup tools
 */
namespace StringTools {

/**
 * Splits string to smaller substrings which have between them a delimiter. Resulting substrings won't have
 * delimiter.
 * @param str Source string
 * @param delimiter Delimiter
 * @param dest Array to which substrings will be saved.
 */
TGBM_API
void split(const std::string& str, char delimiter, std::vector<std::string>& dest);

/**
 * Generates pseudo random string. It's recommended to call srand before this method.
 * @param length Length of resulting string.
 */
TGBM_API
std::string generate_multipart_boundary(std::size_t length);

/**
 * Performs url encode.
 * @param value Source url string
 * @param additionalLegitChars Optional. String of chars which will be not encoded in source url string.
 * @return Encoded url string
 */
void urlEncode(std::string_view value, std::string& out);

struct url_encoded {
  std::string_view str;
};

/**
 * Performs url decode.
 * @param value Encoded url string
 * @return Decoded url string
 */
TGBM_API
std::string urlDecode(const std::string& value);

/**
 * Splits string to smaller substrings which have between them a delimiter. Resulting substrings won't have
 * delimiter.
 * @param str Source string
 * @param delimiter Delimiter
 * @return Array of substrings
 */
inline std::vector<std::string> split(const std::string& str, char delimiter) {
  std::vector<std::string> result;
  split(str, delimiter, result);
  return result;
}

}  // namespace StringTools

namespace fmt {

template <>
struct fmt::formatter<::StringTools::url_encoded> {
  static constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.end();
  }

  static auto format(::StringTools::url_encoded s, auto& ctx) -> decltype(ctx.out()) {
    auto is_legit = [](char c) {
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
    auto it = ctx.out();
    for (auto const& c : s.str) {
      if (is_legit(c)) {
        *it = c;
        ++it;
      } else
        it = fmt::format_to(it, "%{:02X}", (unsigned)(unsigned char)c);
    }
    return it;
  }
};

}  // namespace fmt
