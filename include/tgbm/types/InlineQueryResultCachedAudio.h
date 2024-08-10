#pragma once

#include "tgbm/types/InlineQueryResult.h"
#include "tgbm/types/MessageEntity.h"
#include "tgbm/types/InputMessageContent.h"

#include <memory>
#include <string>
#include <vector>

namespace tgbm {

/**
 * @brief Represents a link to an MP3 audio file stored on the Telegram servers.
 *
 * By default, this audio file will be sent by the user.
 * Alternatively, you can use inputMessageContent to send a message with the specified content instead of the
 * audio.
 *
 * @ingroup types
 */
class InlineQueryResultCachedAudio : public InlineQueryResult {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<InlineQueryResultCachedAudio> Ptr;

  InlineQueryResultCachedAudio() {
    this->type = TYPE;
  }

  /**
   * @brief A valid file identifier for the audio file
   */
  std::string audioFileId;

  /**
   * @brief Optional. Caption, 0-1024 characters after entities parsing
   */
  std::string caption;

  /**
   * @brief Optional. Mode for parsing entities in the audio caption.
   *
   * See [formatting options](https://core.telegram.org/bots/api#formatting-options) for more details.
   */
  std::string parseMode;

  /**
   * @brief Optional. List of special entities that appear in the caption, which can be specified instead of
   * parseMode
   */
  std::vector<MessageEntity::Ptr> captionEntities;

  /**
   * @brief Optional. Content of the message to be sent instead of the audio
   */
  InputMessageContent::Ptr inputMessageContent;
};
}  // namespace tgbm
