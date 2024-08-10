#include "tgbm/tools/StringTools.h"

#include <iomanip>
#include <cstdio>
#include <random>
#include <string>

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

string urlEncode(const string& value, const std::string& additionalLegitChars) {
  // о господи, уж лучше в json передавать чем вот это
  // TODO use application/json
  static const string legitPunctuation =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.-~:";
  std::stringstream ss;
  for (auto const& c : value) {
    if ((legitPunctuation.find(c) == std::string::npos) &&
        (additionalLegitChars.find(c) == std::string::npos)) {
      ss << '%' << std::uppercase << std::setfill('0') << std::setw(2) << std::hex
         << (unsigned int)(unsigned char)c;
    } else {
      ss << c;
    }
  }

  return ss.str();
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
