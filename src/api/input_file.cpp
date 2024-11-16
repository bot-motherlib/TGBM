#include <tgbm/api/input_file.hpp>

#include <fmt/core.h>

#include <fstream>
// TODO boost filesystem + mapped file

namespace tgbm::api {

InputFile InputFile::from_file(std::filesystem::path file_path, std::string mime_type) {
  InputFile f;
  f.mimetype = std::move(mime_type);
  f.filename = file_path.filename().string();
  std::ifstream file(file_path, std::ios::in | std::ios::binary);
  if (!file.is_open())
    throw std::runtime_error(fmt::format("cannot open file, path: {}", file_path.string()));
  std::ostringstream content_stream;
  content_stream << file.rdbuf();
  f.data = std::move(content_stream).str();
  return f;
}

}  // namespace tgbm::api
