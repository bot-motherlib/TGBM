#include "tgbm/tools/StringTools.h"

#include <fmt/format.h>
#include <cstdio>
#include <random>
#include <string>
#include <sstream>

using namespace std;

namespace StringTools {

void split(const string& str, char delimiter, vector<string>& dest) {
  istringstream stream(str);
  string s;
  while (getline(stream, s, delimiter)) {
    dest.push_back(s);
  }
}

string generateRandomString(std::size_t length) {
  static const string chars(
      "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890-=[]\\',./!@#$%^&*()_+{}|:\"<>?`~");

  static const std::size_t charsLen = chars.length();
  string result;

  random_device randomDevice;
  mt19937 randomSeed(randomDevice());
  uniform_int_distribution<std::size_t> generator(0, charsLen - 1);

  for (std::size_t i = 0; i < length; ++i) {
    result += chars[generator(randomSeed)];
  }
  return result;
}

string urlEncode(std::string_view value) {
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
  std::string ss;
  ss.reserve(value.size());
  for (auto const& c : value) {
    if (is_legit(c))
      ss.push_back(c);
    else [[unlikely]]
      fmt::format_to(std::back_inserter(ss), "%{:02X}", (unsigned)(unsigned char)c);
  }
  return ss;
}

string urlDecode(const string& value) {
  string result;
  for (std::size_t i = 0, count = value.length(); i < count; ++i) {
    const char c = value[i];
    if (c == '%') {
      int t = stoi(value.substr(i + 1, 2), nullptr, 16);
      result += (char)t;
      i += 2;
    } else {
      result += c;
    }
  }
  return result;
}

std::string escapeJsonString(const std::string& value) {
  string result;

  for (const char& c : value) {
    switch (c) {
      case '"':
      case '\\':
      case '/':
        result += '\\';
        break;
    }

    result += c;
  }

  return result;
}

}  // namespace StringTools
