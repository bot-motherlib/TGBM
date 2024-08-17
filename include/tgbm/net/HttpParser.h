#pragma once

#include "tgbm/net/Url.h"
#include "tgbm/net/HttpReqArg.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace tgbm {

// TODO wtf это всё статические функции вообще
class TGBM_API HttpParser {
 public:
  std::string generateRequest(const Url& url, const std::vector<HttpReqArg>& args,
                              bool isKeepAlive = false) const;
  static std::string generateApplicationJson(const std::vector<HttpReqArg>& args);
  std::string generateMultipartFormData(const std::vector<HttpReqArg>& args,
                                        const std::string& boundary) const;
  std::string generateMultipartBoundary(const std::vector<HttpReqArg>& args) const;
  std::string generateWwwFormUrlencoded(const std::vector<HttpReqArg>& args) const;
  std::string generateResponse(const std::string& data, const std::string& mimeType,
                               unsigned short statusCode, const std::string& statusStr,
                               bool isKeepAlive) const;
  std::unordered_map<std::string, std::string> parseHeader(const std::string& data, bool isRequest) const;
  std::string extractBody(const std::string& data) const;
};

}  // namespace tgbm
