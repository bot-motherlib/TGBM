#include "tgbot/TgException.h"

#include <string>

namespace tgbm {

TgException::TgException(const std::string& description, ErrorCode errorCode)
    : runtime_error(description), errorCode(errorCode) {
}

}  // namespace tgbm
