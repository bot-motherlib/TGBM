#pragma once

#include <string>

namespace tgbm::api {
// TODO better
// TODO use this file instead of deprecated InputFile
struct InputFile {
  std::string filename;
  std::string mimetype;
  std::string data;

  // TODO impl
  static InputFile fromFile(const std::string& filePath, const std::string& mimeType);
};

}  // namespace tgbm::api
