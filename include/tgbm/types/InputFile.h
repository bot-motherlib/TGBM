#pragma once

#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief This object represents the contents of a file to be uploaded.
 *
 * @ingroup types
 */
class InputFile {
 public:
  typedef std::shared_ptr<InputFile> Ptr;

  /**
   * @brief Contents of a file.
   */
  std::string data;

  /**
   * @brief Mime type of a file.
   */
  std::string mimeType;

  /**
   * @brief File name.
   */
  std::string fileName;

  /**
   * @brief Creates new InputFile::Ptr from an existing file.
   */
  static InputFile::Ptr fromFile(const std::string& filePath, const std::string& mimeType);
};

}  // namespace tgbm
