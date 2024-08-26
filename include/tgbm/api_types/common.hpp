#pragma once

#include <string>
#include <vector>

#include "tgbm/api_types/Integer.hpp"
#include "tgbm/api_types/optional.hpp"

namespace tgbm::api {

using String = std::string;
using Boolean = bool;
using True = std::true_type;

template <typename T>
using arrayof = std::vector<T>;

}  // namespace tgbm::api
