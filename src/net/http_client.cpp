#include "tgbm/net/HttpClient.h"

namespace tgbm {

HttpClient::HttpClient(std::string_view host) : host(host) {
  if (host.empty() || host.starts_with("https://") || host.starts_with("http://"))
    throw std::invalid_argument("host should not be full url, correct example: \"api.telegram.org\"");
}

}  // namespace tgbm
