#include "tgbm/net/HttpParser.h"

#include "tgbm/tools/scope_exit.h"

#include <boost/algorithm/string.hpp>

#include <cstddef>
#include <fmt/format.h>
#include <string>
#include <unordered_map>

#include <rapidjson/document.h>

using namespace std;
using namespace boost;

namespace tgbm {

string HttpParser::generateResponse(const string& data, const string& mimeType, unsigned short statusCode,
                                    const string& statusStr, bool isKeepAlive) const {
  // TODO по сути из statusCode выводится statusStr, не нужно это отдельным аргументом
  string result;
  result += "HTTP/1.1 ";
  result += std::to_string(statusCode);
  result += ' ';
  result += statusStr;
  result += "\r\nContent-Type: ";
  result += mimeType;
  result += "\r\nContent-Length: ";
  result += std::to_string(data.length());
  result += "\r\nConnection: ";
  if (isKeepAlive) {
    result += "keep-alive";
  } else {
    result += "close";
  }
  result += "\r\n\r\n";
  result += data;
  return result;
}

unordered_map<string, string> HttpParser::parseHeader(const string& data, bool isRequest) const {
  unordered_map<string, string> headers;

  std::size_t lineStart = 0;
  std::size_t lineEnd = 0;
  std::size_t lineSepPos = 0;
  std::size_t lastLineEnd = string::npos;
  while (lastLineEnd != lineEnd) {
    lastLineEnd = lineEnd;
    bool isFirstLine = lineEnd == 0;
    if (isFirstLine) {
      if (isRequest) {
        lineSepPos = data.find(' ');
        lineEnd = data.find("\r\n");
        headers["_method"] = data.substr(0, lineSepPos);
        headers["_path"] = data.substr(lineSepPos + 1, data.find(' ', lineSepPos + 1) - lineSepPos - 1);
      } else {
        lineSepPos = data.find(' ');
        lineEnd = data.find("\r\n");
        headers["_status"] = data.substr(lineSepPos + 1, data.find(' ', lineSepPos + 1) - lineSepPos - 1);
      }
    } else {
      lineStart = lineEnd;
      lineStart += 2;
      lineEnd = data.find("\r\n", lineStart);
      lineSepPos = data.find(':', lineStart);
      if (lastLineEnd == lineEnd || lineEnd == string::npos) {
        break;
      }
      headers[data.substr(lineStart, lineSepPos - lineStart)] =
          trim_copy(data.substr(lineSepPos + 1, lineEnd - lineSepPos - 1));
    }
  }

  return headers;
}

}  // namespace tgbm
