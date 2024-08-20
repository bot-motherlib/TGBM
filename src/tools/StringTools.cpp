#include "tgbm/tools/StringTools.h"

#include <fmt/format.h>
#include <cstdio>
#include <random>
#include <string>
#include <sstream>
#include <thread>

#include <boost/functional/hash.hpp>

using namespace std;

namespace tgbm::utils {

void split(const string& str, char delimiter, vector<string>& dest) {
  istringstream stream(str);
  string s;
  while (getline(stream, s, delimiter)) {
    dest.push_back(s);
  }
}

static std::size_t generate_random_seed() noexcept {
  size_t seed = std::random_device{}();
  boost::hash_combine(seed, std::hash<std::thread::id>{}(std::this_thread::get_id()));
  boost::hash_combine(seed, std::chrono::high_resolution_clock::now().time_since_epoch().count());
  return seed;
}

string generate_multipart_boundary(std::size_t length) {
  constexpr std::string_view chars = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890+=()?_";
  string result;
  result.reserve(length);
  thread_local mt19937 gen(generate_random_seed());
  uniform_int_distribution<std::size_t> dist(0, chars.size() - 1);
  for (std::size_t i = 0; i < length; ++i)
    result += chars[dist(gen)];
  return result;
}

void urlEncode(std::string_view value, std::string& out) {
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
  out.reserve(value.size());
  for (auto const& c : value) {
    if (is_legit(c))
      out.push_back(c);
    else [[unlikely]]
      fmt::format_to(std::back_inserter(out), "%{:02X}", (unsigned)(unsigned char)c);
  }
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

}  // namespace tgbm::utils
