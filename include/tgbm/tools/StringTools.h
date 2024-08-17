#pragma once

#include "tgbm/export.h"

#include <cstddef>
#include <vector>
#include <string>

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
std::string generateRandomString(std::size_t length);

/**
 * Performs url encode.
 * @param value Source url string
 * @param additionalLegitChars Optional. String of chars which will be not encoded in source url string.
 * @return Encoded url string
 */
TGBM_API
std::string urlEncode(std::string_view value);

/**
 * Performs url decode.
 * @param value Encoded url string
 * @return Decoded url string
 */
TGBM_API
std::string urlDecode(const std::string& value);

/**
 * Escapes a string with illegal characters ("\/) for json
 *
 * @param value input string
 *
 * @return An encoded string
 */
std::string escapeJsonString(const std::string& value);

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
