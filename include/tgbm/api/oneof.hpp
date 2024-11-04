#pragma once

#include "tgbm/tools/box_union.hpp"

namespace tgbm::api {

template <typename... Types>
using oneof = box_union<Types...>;

}
