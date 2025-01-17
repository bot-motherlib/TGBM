#pragma once

#include <string>
#include <string_view>

#include <tgbm/api/const_string.hpp>
#include <tgbm/utils/string_switch.hpp>
#include <tgbm/utils/box.hpp>
#include <tgbm/utils/box_union.hpp>
#include <tgbm/api/optional.hpp>
#include <tgbm/api/Integer.hpp>
#include <tgbm/api/floating.hpp>
#include <tgbm/utils/pfr_extension.hpp>
#include <tgbm/api/file_or_str.hpp>
#include <tgbm/api/reply_markup.hpp>
#include <tgbm/api/int_or_str.hpp>
#include <tgbm/net/http_base.hpp>
#include <tgbm/api/input_file.hpp>
#include <tgbm/api/arrayof.hpp>
#include <tgbm/api/oneof.hpp>
#include <tgbm/api/true.hpp>
#include <tgbm/net/http_body_builders.hpp>

namespace tgbm::api {

enum struct file_info_e {
  yes,
  no,
  maybe,
};

using String = std::string;

}  // namespace tgbm::api
