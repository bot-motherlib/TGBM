#pragma once

#include <string>
#include <filesystem>

namespace tgbm::api {
// TODO better
// TODO use this file instead of deprecated InputFile
struct InputFile {
  std::string filename;
  std::string mimetype;
  std::string data;

  static InputFile from_file(std::filesystem::path file_path, std::string mime_type);
};

}  // namespace tgbm::api
