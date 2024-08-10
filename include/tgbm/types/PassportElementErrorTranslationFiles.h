#ifndef TGBOT_PASSPORTELEMENTERRORTRANSLATIONFILES_H
#define TGBOT_PASSPORTELEMENTERRORTRANSLATIONFILES_H

#include "tgbm/types/PassportElementError.h"

#include <string>
#include <memory>
#include <vector>

namespace tgbm {

/**
 * @brief Represents an issue with the translated version of a document.
 * The error is considered resolved when a file with the document translation change.
 *
 * @ingroup types
 */
class PassportElementErrorTranslationFiles : public PassportElementError {
 public:
  static const std::string SOURCE;

  typedef std::shared_ptr<PassportElementErrorTranslationFiles> Ptr;

  PassportElementErrorTranslationFiles() {
    this->source = SOURCE;
  }

  /**
   * @brief List of base64-encoded file hashes
   */
  std::vector<std::string> fileHashes;
};
}  // namespace tgbm

#endif  // TGBOT_PASSPORTELEMENTERRORTRANSLATIONFILES_H
