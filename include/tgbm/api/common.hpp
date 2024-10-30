#pragma once

#include <compare>
#include <string>
#include <vector>
#include <string_view>

#include "tgbm/tools/StringTools.h"
#include "tgbm/tools/box.hpp"
#include "tgbm/tools/box_union.hpp"
#include "tgbm/api/optional.hpp"
#include "tgbm/api/Integer.hpp"
#include "tgbm/api/floating.hpp"
#include "tgbm/tools/pfr_extension.hpp"
#include "tgbm/api/file_or_str.hpp"
#include "tgbm/api/reply_markup.hpp"
#include "tgbm/api/int_or_str.hpp"
#include "tgbm/net/http_base.hpp"

namespace tgbm::api {

enum struct file_info_e {
  yes,
  no,
  maybe,
};

using String = std::string;
using Boolean = bool;

struct True {
  constexpr std::strong_ordering operator<=>(const True&) const = default;
};

template <typename T>
using arrayof = std::vector<T>;

template <typename... U>
using oneof = box_union<U...>;

struct placeholder {
  consteval static bool is_optional_field(std::string_view) {
    return false;
  }
};

}  // namespace tgbm::api
