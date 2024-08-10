#pragma once

#include "tgbm/types/PassportElementError.h"

#include <string>
#include <memory>
#include <vector>

namespace tgbm {

/**
 * @brief Represents an issue with a list of scans.
 * The error is considered resolved when the list of files containing the scans changes.
 *
 * @ingroup types
 */
class PassportElementErrorFiles : public PassportElementError {
 public:
  static const std::string SOURCE;

  typedef std::shared_ptr<PassportElementErrorFiles> Ptr;

  PassportElementErrorFiles() {
    this->source = SOURCE;
  }

  /**
   * @brief List of base64-encoded file hashes
   */
  std::vector<std::string> fileHashes;
};
}  // namespace tgbm
