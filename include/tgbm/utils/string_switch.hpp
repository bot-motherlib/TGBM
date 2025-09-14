#pragma once

#include <strswitch/strswitch.hpp>

namespace tgbm {

template <typename T, typename R = T>
using string_switch = ss::string_switch<T, R>;

}  // namespace tgbm
