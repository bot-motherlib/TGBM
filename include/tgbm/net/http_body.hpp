#pragma once

#include <string>
#include <vector>

namespace tgbm {

using bytes_t = std::vector<uint8_t>;

struct http_body {
  std::string content_type;
  bytes_t data;
};

}  // namespace tgbm
