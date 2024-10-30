#pragma once

#include <string>

// TODO rm
#include "tgbm/types/InputFile.h"

namespace tgbm::api {

// TODO better
// TODO use this file instead of deprecated InputFile
struct InputFile {
  std::string filename;
  std::string mimetype;
  std::string data;
};

}  // namespace tgbm::api
