#pragma once

#include "tgbm/export.h"

#include <string>

/**
 * @ingroup tools
 */
namespace FileTools {

/**
 * Reads whole file to string.
 * @param filePath Path to a file
 * @throws exception of type std::ifstream::failure if reading fails
 * @return string with file contents
 */
TGBM_API
std::string read(const std::string& filePath);

/**
 * Save file to disk.
 * @param filePath Path to a file
 * @throws exception of type std::ifstream::failure if writing fails
 */
TGBM_API
void write(const std::string& content, const std::string& filePath);

}  // namespace FileTools
