#pragma once

#include <string>
#include <filesystem>

namespace tgbm::api {

struct InputFile {
  std::string filename;
  std::string mimetype;
  std::string data;

  static InputFile from_file(std::filesystem::path file_path, std::string mime_type);
};

}  // namespace tgbm::api
