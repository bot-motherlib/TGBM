#ifndef TGBOT_PASSPORTELEMENTERRORFILE_H
#define TGBOT_PASSPORTELEMENTERRORFILE_H

#include "tgbm/types/PassportElementError.h"

#include <string>
#include <memory>

namespace tgbm {

/**
 * @brief Represents an issue with a document scan.
 * The error is considered resolved when the file with the document scan changes.
 *
 * @ingroup types
 */
class PassportElementErrorFile : public PassportElementError {
 public:
  static const std::string SOURCE;

  typedef std::shared_ptr<PassportElementErrorFile> Ptr;

  PassportElementErrorFile() {
    this->source = SOURCE;
  }

  /**
   * @brief Base64-encoded file hash
   */
  std::string fileHash;
};
}  // namespace tgbm

#endif  // TGBOT_PASSPORTELEMENTERRORFILE_H
