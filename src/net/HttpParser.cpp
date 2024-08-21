#include "tgbm/net/HttpParser.h"

#include "tgbm/scope_exit.h"

#include <boost/algorithm/string.hpp>

#include <cstddef>
#include <fmt/format.h>
#include <string>
#include <unordered_map>

#include <rapidjson/document.h>

using namespace std;
using namespace boost;

namespace tgbm {

std::string generate_http_headers_get(const tg_url_view& url, bool keep_alive) {
  // TODO optimized inlined version
  return generate_http_headers(url, keep_alive, {}, {});
}
std::string generate_http_headers(const tg_url_view& url, bool keep_alive, std::string_view body,
                                  std::string_view content_type) {
  assert(body != "{}" && "empty json object provided");
  std::string body_sz_str = fmt::format("{}", body.size());
  size_t size_bytes = [&] {
    using namespace std::string_view_literals;
    size_t headers_size = 0;
    headers_size += body.empty() ? "GET "sv.size() : "POST "sv.size();
    headers_size += url.path.size() + url.method.size();
    headers_size += " HTTP/1.1\r\nHost: "sv.size() + url.host.size() + "\r\nConnection: "sv.size() +
                    (keep_alive ? "keep-alive"sv.size() : "close"sv.size());
    headers_size += "\r\n"sv.size();
    if (body.empty()) {
      headers_size += "\r\n"sv.size();
      return headers_size;
    }
    headers_size += "Content-Type: "sv.size() + content_type.size() + "\r\nContent-Length: "sv.size() +
                    body_sz_str.size() + "\r\n\r\n"sv.size();
    return headers_size;
  }();
  string result;
  // TODO resize + format to char* (separate GET / POST)
  result.reserve(size_bytes);
  on_scope_exit {
    assert(result.size() == size_bytes && "not exactly equal size reserved, logical error");
  };
  if (body.empty())
    result += "GET ";
  else
    result += "POST ";
  result += url.path;
  result += url.method;
  result +=
      " HTTP/1.1\r\n"
      "Host: ";
  result += url.host;
  result += "\r\nConnection: ";
  if (keep_alive)
    result += "keep-alive";
  else
    result += "close";
  result += "\r\n";
  if (body.empty()) {
    result += "\r\n";
    return result;
  }
  result += "Content-Type: ";
  result += content_type;
  result += "\r\nContent-Length: ";
  result += body_sz_str;
  result += "\r\n\r\n";
  return result;
}

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
