#include "tgbm/types/PassportElementErrorDataField.h"
#include "tgbm/types/PassportElementErrorFrontSide.h"
#include "tgbm/types/PassportElementErrorReverseSide.h"
#include "tgbm/types/PassportElementErrorSelfie.h"
#include "tgbm/types/PassportElementErrorFile.h"
#include "tgbm/types/PassportElementErrorFiles.h"
#include "tgbm/types/PassportElementErrorTranslationFile.h"
#include "tgbm/types/PassportElementErrorTranslationFiles.h"
#include "tgbm/types/PassportElementErrorUnspecified.h"

#include <string>

using namespace tgbm;

const std::string PassportElementErrorDataField::SOURCE = "data";
const std::string PassportElementErrorFrontSide::SOURCE = "front_side";
const std::string PassportElementErrorReverseSide::SOURCE = "reverse_side";
const std::string PassportElementErrorSelfie::SOURCE = "selfie";
const std::string PassportElementErrorFile::SOURCE = "file";
const std::string PassportElementErrorFiles::SOURCE = "files";
const std::string PassportElementErrorTranslationFile::SOURCE = "translation_file";
const std::string PassportElementErrorTranslationFiles::SOURCE = "translation_files";
const std::string PassportElementErrorUnspecified::SOURCE = "unspecified";
