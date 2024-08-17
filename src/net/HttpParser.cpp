#include "tgbm/net/HttpParser.h"

#include "tgbm/tools/StringTools.h"

#include <boost/algorithm/string.hpp>

#include <cstddef>
#include <fmt/format.h>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;
using namespace boost;

namespace tgbm {

// TODO rapid json (escape all)
std::string HttpParser::generateApplicationJson(const std::vector<HttpReqArg>& args) {
  assert(!args.empty());
  string result;
  // TODO reserve exactly? item.name.size() + item.value.size() + 4 quotes + comma (-1) + object { }
  result.reserve(args.size() * 20);
  auto bi = std::back_inserter(result);
  result.push_back('{');
  auto b = args.begin();
  auto e = args.end();
  goto start;
  for (; b != e; ++b) {
    result.push_back(',');
  start:
    // TODO appends?
    fmt::format_to(bi, R"("{}":"{}")", b->name, b->value);
  }
  result.push_back('}');
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
      // TODO application/json ?
      result += "Content-Type: application/x-www-form-urlencoded\r\n";
      requestData = generateWwwFormUrlencoded(args);
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
