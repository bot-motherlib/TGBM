#pragma once

#include <variant>

#include <tgbm/api/input_file.hpp>

namespace tgbm::api {

struct file_or_str {
  std::variant<std::monostate, InputFile, std::string> data;

  file_or_str() = default;

  file_or_str(std::string str) noexcept : data(std::move(str)) {
  }
  file_or_str(InputFile file) noexcept : data(std::move(file)) {
  }

  // returns true if contains file or string
  explicit operator bool() const noexcept {
    return data.index() != 0;
  }

  std::string* get_str() noexcept {
    return std::get_if<std::string>(&data);
  }
  const std::string* get_str() const noexcept {
    return std::get_if<std::string>(&data);
  }
  bool is_str() const noexcept {
    return !!get_str();
  }

  InputFile* get_file() noexcept {
    return std::get_if<InputFile>(&data);
  }
  const InputFile* get_file() const noexcept {
    return std::get_if<InputFile>(&data);
  }
  bool is_file() const noexcept {
    return !!get_file();
  }
};

}  // namespace tgbm::api
