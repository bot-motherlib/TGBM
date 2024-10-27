#pragma once

#include <variant>
#include <string>
#include <cstdint>

namespace tgbm::api {

// TODO optional for this type (or is setted instead?..)
using int_or_str = std::variant<int64_t, std::string>;

}  // namespace tgbm::api
