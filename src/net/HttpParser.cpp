#include "tgbm/net/HttpParser.h"

#include "tgbm/scope_exit.h"
#include "tgbm/tools/StringTools.h"

#include <boost/algorithm/string.hpp>

#include <cstddef>
#include <fmt/format.h>
#include <string>
#include <vector>
#include <unordered_map>

#include <rapidjson/document.h>

using namespace std;
using namespace boost;

namespace tgbm {

std::string HttpParser::generateApplicationJson(const std::vector<HttpReqArg>& args) {
  struct rapidjson_string_buffer {
    std::string& buf;

    using Ch = char;

    void Put(char c) {
      buf.push_back(c);
    }
    static void Flush() noexcept {
    }
  };

  std::string result;
  if (args.empty())
    return result;
  result.reserve(args.size() * 20);  // TODO may be calculate better
  rapidjson_string_buffer buffer(result);
  rapidjson::Writer writer(buffer);
  writer.StartObject();
  for (auto& arg : args) {
    writer.Key(arg.name);
    writer.String(arg.value);
  }
  writer.EndObject();
  return result;
}

std::string generate_http_headers_get(const Url& url, bool keep_alive) {
  // TODO optimized inlined version
  return generate_http_headers(url, keep_alive, {}, {});
}
std::string generate_http_headers(const Url& url, bool keep_alive, std::string_view body,
                                  std::string_view content_type) {
  assert(body != "{}" && "empty json object provided");
  std::string body_sz_str = fmt::format("{}", body.size());
  size_t size_bytes = [&] {
    using namespace std::string_view_literals;
    size_t headers_size = 0;
    headers_size += body.empty() ? "GET "sv.size() : "POST "sv.size();
    headers_size += url.path.size();
    if (!url.query.empty())
      headers_size += url.query.size() + 1;  // "?"
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
  // TODO resize
  result.reserve(size_bytes);
  on_scope_exit {
    assert(result.size() == size_bytes && "not exactly equal size reserved, logical error");
  };
  if (body.empty())
    result += "GET ";
  else
    result += "POST ";
  result += url.path;
  result += url.query.empty() ? "" : "?" + url.query;
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

string HttpParser::generateRequest(const Url& url, const vector<HttpReqArg>& args, bool isKeepAlive) const {
  string result;
  if (args.empty()) {
    result += "GET ";
  } else {
    result += "POST ";
  }
  result += url.path;
  result += url.query.empty() ? "" : "?" + url.query;
  result += " HTTP/1.1\r\n";
  result += "Host: ";
  result += url.host;
  result += "\r\nConnection: ";
  if (isKeepAlive) {
    result += "keep-alive";
  } else {
    result += "close";
  }
  result += "\r\n";
  if (args.empty()) {
    result += "\r\n";
  } else {
    string requestData;

    string bondary = generateMultipartBoundary(args);
    if (bondary.empty()) {
      result += "Content-Type: application/json\r\n";
      requestData = generateApplicationJson(args);
    } else {
      result += "Content-Type: multipart/form-data; boundary=";
      result += bondary;
      result += "\r\n";
      requestData = generateMultipartFormData(args, bondary);
    }

    result += "Content-Length: ";
    result += std::to_string(requestData.length());
    result += "\r\n\r\n";
    result += requestData;
  }
  return result;
}
// TODO используется когда хотя бы 1 аргумент - файл, нужно обдумать когда это вообще
string HttpParser::generateMultipartFormData(const vector<HttpReqArg>& args, const string& boundary) const {
  string result;
  for (const HttpReqArg& item : args) {
    result += "--";
    result += boundary;
    result += "\r\nContent-Disposition: form-data; name=\"";
    result += item.name;
    if (item.isFile) {
      result += "\"; filename=\"" + item.fileName;
    }
    result += "\"\r\n";
    if (item.isFile) {
      result += "Content-Type: ";
      result += item.mimeType;
      result += "\r\n";
    }
    result += "\r\n";
    result += item.value;
    result += "\r\n";
  }
  result += "--" + boundary + "--\r\n";
  return result;
}

string HttpParser::generateMultipartBoundary(const vector<HttpReqArg>& args) const {
  string result;
  // TODO нужно смотреть что вообще тут происходит по http спеке, странное что-то
  // насколько понял boundary должно не содержаться в файле и стоит в начале и конце, и так он генерит его..
  for (const HttpReqArg& item : args) {
    if (item.isFile) {
      while (result.empty() || item.value.find(result) != string::npos) {
        result += StringTools::generateRandomString(4);
      }
    }
  }
  return result;
}

string HttpParser::generateWwwFormUrlencoded(const vector<HttpReqArg>& args) const {
  string result;

  bool firstRun = true;
  for (const HttpReqArg& item : args) {
    if (firstRun) {
      firstRun = false;
    } else {
      result += '&';
    }
    result += StringTools::urlEncode(item.name);
    result += '=';
    result += StringTools::urlEncode(item.value);
  }

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

string HttpParser::extractBody(const string& data) const {
  std::size_t headerEnd = data.find("\r\n\r\n");
  if (headerEnd == string::npos) {
    return data;
  }
  headerEnd += 4;
  return data.substr(headerEnd);
}

}  // namespace tgbm
