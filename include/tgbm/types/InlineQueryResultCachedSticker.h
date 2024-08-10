#pragma once

#include "tgbm/types/InlineQueryResult.h"
#include "tgbm/types/InputMessageContent.h"

#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief Represents a link to a sticker stored on the Telegram servers.
 *
 * By default, this sticker will be sent by the user.
 * Alternatively, you can use inputMessageContent to send a message with the specified content instead of the
 * sticker.
 *
 * @ingroup types
 */
class InlineQueryResultCachedSticker : public InlineQueryResult {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<InlineQueryResultCachedSticker> Ptr;

  InlineQueryResultCachedSticker() {
    this->type = TYPE;
  }

  /**
   * @brief A valid file identifier of the sticker
   */
  std::string stickerFileId;

  /**
   * @brief Optional. Content of the message to be sent instead of the sticker
   */
  InputMessageContent::Ptr inputMessageContent;
};
}  // namespace tgbm
