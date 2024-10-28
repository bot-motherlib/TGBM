#pragma once

#include <variant>

// TODO use instead of InputFile #include "tgbm/api/input_file.hpp"
#include "tgbm/types/InputFile.h"

namespace tgbm::api {

using file_or_str = std::variant<InputFile::Ptr, std::string>;

}  // namespace tgbm::api
