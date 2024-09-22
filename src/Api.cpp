#include "tgbm/Api.h"

#include <chrono>

#include <fmt/ranges.h>

#include "tgbm/tools/algorithm.hpp"
#include "tgbm/logger.h"

namespace tgbm {

using default_body_t = application_json_body;

static bool is_empty_chatid(const int_or_str& x) {
  if (auto* i = std::get_if<int64_t>(&x))
    return *i == 0;
  else
    return std::get_if<std::string>(&x)->empty();
}

static bool is_empty_thumbnail(const thumbnail_t& x) {
  if (auto* i = std::get_if<InputFile::Ptr>(&x))
    return *i == nullptr;
  else
    return std::get_if<std::string>(&x)->empty();
}

// name of function should be same as in API https://core.telegram.org/bots/api
#define $METHOD ::std::string_view(__func__)
#define $POST_REQUEST co_await sendRequest(http_request{get_path($METHOD), body.take()}, _timeout)
#define $GET_REQUEST co_await sendRequest(http_request(get_path($METHOD)), _timeout)

Api::Api(std::string token, http_client& httpClient, duration_t timeout)
    : _timeout(timeout), _token(std::move(token)), _httpClient(httpClient) {
}

dd::task<std::vector<Update::Ptr>> Api::getUpdates(std::int32_t offset, std::int32_t limit,
                                                   std::int32_t timeout,
                                                   const StringArrayPtr& allowedUpdates) const {
  // TODO declare structs and default arguments (autogenerate this code)
  // TODO reserve везде в body примерное количество байт (можно и точно посчитать)
  default_body_t body;
  if (offset != 0)
    body.arg("offset", offset);
  if (limit != 100)
    body.arg("limit", std::max(1, std::min(100, limit)));
  if (timeout != 0)
    body.arg("timeout", timeout);
  if (allowedUpdates)
    body.arg("allowed_updates", *allowedUpdates);
  // sets timeout to be greater then long pool timeout
  boost::property_tree::ptree json = co_await sendRequest({.path = get_path($METHOD), .body = body.take()},
                                                          _timeout + std::chrono::seconds(timeout));
  co_return _tgTypeParser.parseJsonAndGetArray<Update>(&TgTypeParser::parseJsonAndGetUpdate, json);
}

dd::task<bool> Api::setWebhook(const std::string& url, InputFile::Ptr certificate,
                               std::int32_t maxConnections, const StringArrayPtr& allowedUpdates,
                               const std::string& ipAddress, bool dropPendingUpdates,
                               const std::string& secretToken) const {
  boost::property_tree::ptree json;
  auto fillbody = [&](auto& body) {
    body.arg("url", url);
    if (!ipAddress.empty())
      body.arg("ip_address", ipAddress);
    if (maxConnections != 40)
      body.arg("max_connections", std::max(1, std::min(100, maxConnections)));
    if (allowedUpdates)
      body.arg("allowed_updates", *allowedUpdates);
    if (dropPendingUpdates)
      body.arg("drop_pending_updates", dropPendingUpdates);
    if (!secretToken.empty())
      body.arg("secret_token", secretToken);
  };
  if (certificate) {
    application_multipart_form_data body;
    body.file_arg("certificate", certificate->mimeType, certificate->fileName, certificate->data);
    fillbody(body);
    json = $POST_REQUEST;
  } else {
    default_body_t body;
    fillbody(body);
    json = $POST_REQUEST;
  }
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::deleteWebhook(bool dropPendingUpdates) const {
  default_body_t body;
  if (dropPendingUpdates)
    body.arg("drop_pending_updates", dropPendingUpdates);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<WebhookInfo::Ptr> Api::getWebhookInfo() const {
  boost::property_tree::ptree p = $GET_REQUEST;

  if (!p.get_child_optional("url"))
    co_return nullptr;

  if (p.get<std::string>("url", "") != "")
    co_return _tgTypeParser.parseJsonAndGetWebhookInfo(p);
  co_return nullptr;
}

dd::task<User::Ptr> Api::getMe() const {
  boost::property_tree::ptree json = $GET_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetUser(json);
}

dd::task<bool> Api::logOut() const {
  boost::property_tree::ptree json = $GET_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::close() const {
  boost::property_tree::ptree json = $GET_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<std::vector<Message::Ptr>> Api::sendMessage(
    int_or_str chatId, std::string text, LinkPreviewOptions::Ptr linkPreviewOptions,
    ReplyParameters::Ptr replyParameters, GenericReply::Ptr replyMarkup, std::string parseMode,
    bool disableNotification, std::vector<MessageEntity::Ptr> entities, std::int32_t messageThreadId,
    bool protectContent, std::string businessConnectionId) const {
  // TODO а что если отправить пустое сообщение или одни пробелы? Нужно здесь это обработать
  // TODO обрезать пробелы с двух сторон
  // TODO better
  enum { max_tg_msg_size = 4096 };
  std::vector<Message::Ptr> msgs;
  if (text.size() == 0)
    co_return msgs;
  while (text.size() > max_tg_msg_size) {
    // TODO should i repeat linkPreviewOptions etc for all messages? entities?
    std::vector msg = co_await sendMessage(
        chatId, text.substr(0, max_tg_msg_size), linkPreviewOptions, replyParameters, replyMarkup, parseMode,
        disableNotification, std::move(entities), messageThreadId, protectContent, businessConnectionId);
    assert(msg.size() == 1);
    msgs.push_back(std::move(msg.front()));
    text = text.substr(max_tg_msg_size);
  }
  default_body_t body;
  if (!businessConnectionId.empty())
    body.arg("business_connection_id", businessConnectionId);
  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  body.arg("text", text);
  if (!parseMode.empty())
    body.arg("parse_mode", parseMode);
  if (!entities.empty())
    body.arg("entities",
             _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, entities));
  if (linkPreviewOptions)
    body.arg("link_preview_options", _tgTypeParser.parseLinkPreviewOptions(linkPreviewOptions));
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (replyParameters)
    body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  boost::property_tree::ptree json = $POST_REQUEST;
  msgs.push_back(_tgTypeParser.parseJsonAndGetMessage(json));
  co_return msgs;
}

dd::task<Message::Ptr> Api::forwardMessage(int_or_str chatId, int_or_str fromChatId, std::int32_t messageId,
                                           bool disableNotification, bool protectContent,
                                           std::int32_t messageThreadId) const {
  default_body_t body;
  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  body.arg("from_chat_id", fromChatId);
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  body.arg("message_id", messageId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<std::vector<MessageId::Ptr>> Api::forwardMessages(int_or_str chatId, int_or_str fromChatId,
                                                           const std::vector<std::int32_t>& messageIds,
                                                           std::int32_t messageThreadId,
                                                           bool disableNotification,
                                                           bool protectContent) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("from_chat_id", fromChatId);
  if (!messageIds.empty())
    body.arg("message_ids", messageIds);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetArray<MessageId>(&TgTypeParser::parseJsonAndGetMessageId, json);
}

dd::task<MessageId::Ptr> Api::copyMessage(int_or_str chatId, int_or_str fromChatId, std::int32_t messageId,
                                          const std::string& caption, const std::string& parseMode,
                                          const std::vector<MessageEntity::Ptr>& captionEntities,
                                          bool disableNotification, ReplyParameters::Ptr replyParameters,
                                          GenericReply::Ptr replyMarkup, bool protectContent,
                                          std::int32_t messageThreadId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  body.arg("from_chat_id", fromChatId);
  body.arg("message_id", messageId);
  if (!caption.empty())
    body.arg("caption", caption);
  if (!parseMode.empty())
    body.arg("parse_mode", parseMode);
  // TODO rj_tojson for types/
  if (!captionEntities.empty()) {
    body.arg("caption_entities",
             _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, captionEntities));
  }
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (replyParameters)
    body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessageId(json);
}

dd::task<std::vector<MessageId::Ptr>> Api::copyMessages(int_or_str chatId, int_or_str fromChatId,
                                                        const std::vector<std::int32_t>& messageIds,
                                                        std::int32_t messageThreadId,
                                                        bool disableNotification, bool protectContent,
                                                        bool removeCaption) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("from_chat_id", fromChatId);

  if (!messageIds.empty())
    body.arg("message_ids", messageIds);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (removeCaption)
    body.arg("remove_caption", removeCaption);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetArray<MessageId>(&TgTypeParser::parseJsonAndGetMessageId, json);
}

dd::task<Message::Ptr> Api::sendPhoto(int_or_str chatId, thumbnail_t photo, const std::string& caption,
                                      ReplyParameters::Ptr replyParameters, GenericReply::Ptr replyMarkup,
                                      const std::string& parseMode, bool disableNotification,
                                      const std::vector<MessageEntity::Ptr>& captionEntities,
                                      std::int32_t messageThreadId, bool protectContent, bool hasSpoiler,
                                      const std::string& businessConnectionId) const {
  boost::property_tree::ptree json;
  auto fillbody = [&](auto& body) {
    if (!businessConnectionId.empty())
      body.arg("business_connection_id", businessConnectionId);
    body.arg("chat_id", chatId);
    if (messageThreadId != 0)
      body.arg("message_thread_id", messageThreadId);
    if (!caption.empty())
      body.arg("caption", caption);
    if (!parseMode.empty())
      body.arg("parse_mode", parseMode);
    if (!captionEntities.empty()) {
      body.arg("caption_entities",
               _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, captionEntities));
    }
    if (hasSpoiler)
      body.arg("has_spoiler", hasSpoiler);
    if (disableNotification)
      body.arg("disable_notification", disableNotification);
    if (protectContent)
      body.arg("protect_content", protectContent);
    if (replyParameters)
      body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
    if (replyMarkup)
      body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  };
  if (auto* file = std::get_if<InputFile::Ptr>(&photo)) {
    application_multipart_form_data body;
    assert(*file);
    body.file_arg("photo", file->get()->mimeType, file->get()->fileName, file->get()->data);
    fillbody(body);
    json = $POST_REQUEST;
  } else {
    default_body_t body;
    body.arg("photo", *std::get_if<std::string>(&photo));
    fillbody(body);
    json = $POST_REQUEST;
  }
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::sendAudio(int_or_str chatId, thumbnail_t audio, const std::string& caption,
                                      std::int32_t duration, const std::string& performer,
                                      const std::string& title, thumbnail_t thumbnail,
                                      ReplyParameters::Ptr replyParameters, GenericReply::Ptr replyMarkup,
                                      const std::string& parseMode, bool disableNotification,
                                      const std::vector<MessageEntity::Ptr>& captionEntities,
                                      std::int32_t messageThreadId, bool protectContent,
                                      const std::string& businessConnectionId) const {
  boost::property_tree::ptree json;

  auto fillbody = [&](auto& body) {
    if (!businessConnectionId.empty())
      body.arg("business_connection_id", businessConnectionId);
    body.arg("chat_id", chatId);
    if (messageThreadId != 0)
      body.arg("message_thread_id", messageThreadId);
    if (!caption.empty())
      body.arg("caption", caption);
    if (!parseMode.empty())
      body.arg("parse_mode", parseMode);
    if (!captionEntities.empty())
      body.arg("caption_entities",
               _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, captionEntities));
    if (duration)
      body.arg("duration", duration);
    if (!performer.empty())
      body.arg("performer", performer);
    if (!title.empty())
      body.arg("title", title);
    if (disableNotification)
      body.arg("disable_notification", disableNotification);
    if (protectContent)
      body.arg("protect_content", protectContent);
    if (replyParameters)
      body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
    if (replyMarkup)
      body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  };

  if (audio.index() == 0 || thumbnail.index() == 0) {  // InputFile::Ptr
    application_multipart_form_data body;
    if (auto* file = std::get_if<InputFile::Ptr>(&audio)) {
      assert(*file);
      body.file_arg("audio", file->get()->fileName, file->get()->mimeType, file->get()->data);
    } else {
      body.arg("audio", std::get<std::string>(audio));
    }
    if (auto* file = std::get_if<InputFile::Ptr>(&thumbnail)) {
      assert(*file);
      body.file_arg("thumbnail", file->get()->fileName, file->get()->mimeType, file->get()->data);
    } else {
      auto& str = std::get<std::string>(thumbnail);
      if (!str.empty())
        body.arg("thumbnail", str);
    }
    fillbody(body);
    json = $POST_REQUEST;
  } else {
    default_body_t body;
    body.arg("audio", std::get<std::string>(audio));
    auto& str = *std::get_if<1>(&thumbnail);
    if (!str.empty())
      body.arg("thumbnail", str);
    fillbody(body);
    json = $POST_REQUEST;
  }
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::sendDocument(int_or_str chatId, thumbnail_t document, thumbnail_t thumbnail,
                                         const std::string& caption, ReplyParameters::Ptr replyParameters,
                                         GenericReply::Ptr replyMarkup, const std::string& parseMode,
                                         bool disableNotification,
                                         const std::vector<MessageEntity::Ptr>& captionEntities,
                                         bool disableContentTypeDetection, std::int32_t messageThreadId,
                                         bool protectContent, const std::string& businessConnectionId) const {
  boost::property_tree::ptree json;

  auto fillbody = [&](auto& body) {
    if (!businessConnectionId.empty())
      body.arg("business_connection_id", businessConnectionId);
    body.arg("chat_id", chatId);
    if (messageThreadId != 0)
      body.arg("message_thread_id", messageThreadId);
    if (!caption.empty())
      body.arg("caption", caption);
    if (!parseMode.empty())
      body.arg("parse_mode", parseMode);
    if (!captionEntities.empty()) {
      body.arg("caption_entities",
               _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, captionEntities));
    }
    if (disableContentTypeDetection)
      body.arg("disable_content_type_detection", disableContentTypeDetection);
    if (disableNotification)
      body.arg("disable_notification", disableNotification);
    if (protectContent)
      body.arg("protect_content", protectContent);
    if (replyParameters)
      body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
    if (replyMarkup)
      body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  };

  if (document.index() == 0 || thumbnail.index() == 0) {  // InputFile::Ptr
    application_multipart_form_data body;
    if (auto* file = std::get_if<InputFile::Ptr>(&document)) {
      assert(*file);
      body.file_arg("document", file->get()->fileName, file->get()->mimeType, file->get()->data);
    } else {
      body.arg("document", std::get<std::string>(document));
    }
    if (auto* file = std::get_if<InputFile::Ptr>(&thumbnail)) {
      assert(*file);
      body.file_arg("thumbnail", file->get()->fileName, file->get()->mimeType, file->get()->data);
    } else {
      auto& str = std::get<std::string>(thumbnail);
      if (!str.empty())
        body.arg("thumbnail", str);
    }
    fillbody(body);
    json = $POST_REQUEST;
  } else {
    default_body_t body;
    body.arg("document", std::get<std::string>(document));
    auto& str = *std::get_if<1>(&thumbnail);
    if (!str.empty())
      body.arg("thumbnail", str);
    fillbody(body);
    json = $POST_REQUEST;
  }
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::sendVideo(int_or_str chatId, thumbnail_t video, bool supportsStreaming,
                                      std::int32_t duration, std::int32_t width, std::int32_t height,
                                      thumbnail_t thumbnail, const std::string& caption,
                                      ReplyParameters::Ptr replyParameters, GenericReply::Ptr replyMarkup,
                                      const std::string& parseMode, bool disableNotification,
                                      const std::vector<MessageEntity::Ptr>& captionEntities,
                                      std::int32_t messageThreadId, bool protectContent, bool hasSpoiler,
                                      const std::string& businessConnectionId) const {
  boost::property_tree::ptree json;

  auto fillbody = [&](auto& body) {
    if (!businessConnectionId.empty())
      body.arg("business_connection_id", businessConnectionId);
    body.arg("chat_id", chatId);
    if (messageThreadId != 0)
      body.arg("message_thread_id", messageThreadId);
    if (duration != 0)
      body.arg("duration", duration);
    if (width != 0)
      body.arg("width", width);
    if (height != 0)
      body.arg("height", height);
    if (!caption.empty())
      body.arg("caption", caption);
    if (!parseMode.empty())
      body.arg("parse_mode", parseMode);
    if (!captionEntities.empty()) {
      body.arg("caption_entities",
               _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, captionEntities));
    }
    if (hasSpoiler)
      body.arg("has_spoiler", hasSpoiler);
    if (supportsStreaming)
      body.arg("supports_streaming", supportsStreaming);
    if (disableNotification)
      body.arg("disable_notification", disableNotification);
    if (protectContent)
      body.arg("protect_content", protectContent);
    if (replyParameters)
      body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
    if (replyMarkup)
      body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  };

  if (video.index() == 0 || thumbnail.index() == 0) {  // InputFile::Ptr
    application_multipart_form_data body;
    if (auto* file = std::get_if<InputFile::Ptr>(&video)) {
      assert(*file);
      body.file_arg("video", file->get()->fileName, file->get()->mimeType, file->get()->data);
    } else {
      body.arg("video", std::get<std::string>(video));
    }
    if (auto* file = std::get_if<InputFile::Ptr>(&thumbnail)) {
      assert(*file);
      body.file_arg("thumbnail", file->get()->fileName, file->get()->mimeType, file->get()->data);
    } else {
      auto& str = std::get<std::string>(thumbnail);
      if (!str.empty())
        body.arg("thumbnail", str);
    }
    fillbody(body);
    json = $POST_REQUEST;
  } else {
    default_body_t body;
    body.arg("video", std::get<std::string>(video));
    auto& str = *std::get_if<1>(&thumbnail);
    if (!str.empty())
      body.arg("thumbnail", str);
    fillbody(body);
    json = $POST_REQUEST;
  }
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::sendAnimation(int_or_str chatId, thumbnail_t animation, std::int32_t duration,
                                          std::int32_t width, std::int32_t height, thumbnail_t thumbnail,
                                          const std::string& caption, ReplyParameters::Ptr replyParameters,
                                          GenericReply::Ptr replyMarkup, const std::string& parseMode,
                                          bool disableNotification,
                                          const std::vector<MessageEntity::Ptr>& captionEntities,
                                          std::int32_t messageThreadId, bool protectContent, bool hasSpoiler,
                                          const std::string& businessConnectionId) const {
  boost::property_tree::ptree json;

  auto fillbody = [&](auto& body) {
    if (!businessConnectionId.empty())
      body.arg("business_connection_id", businessConnectionId);
    body.arg("chat_id", chatId);
    if (messageThreadId != 0)
      body.arg("message_thread_id", messageThreadId);
    if (duration != 0)
      body.arg("duration", duration);
    if (width != 0)
      body.arg("width", width);
    if (height != 0)
      body.arg("height", height);
    if (!caption.empty())
      body.arg("caption", caption);
    if (!parseMode.empty())
      body.arg("parse_mode", parseMode);
    if (!captionEntities.empty()) {
      body.arg("caption_entities",
               _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, captionEntities));
    }
    if (hasSpoiler)
      body.arg("has_spoiler", hasSpoiler);
    if (disableNotification)
      body.arg("disable_notification", disableNotification);
    if (protectContent)
      body.arg("protect_content", protectContent);
    if (replyParameters)
      body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
    if (replyMarkup)
      body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  };

  if (animation.index() == 0 || thumbnail.index() == 0) {  // InputFile::Ptr
    application_multipart_form_data body;
    if (auto* file = std::get_if<InputFile::Ptr>(&animation)) {
      assert(*file);
      body.file_arg("animation", file->get()->fileName, file->get()->mimeType, file->get()->data);
    } else {
      body.arg("animation", std::get<std::string>(animation));
    }
    if (auto* file = std::get_if<InputFile::Ptr>(&thumbnail)) {
      assert(*file);
      body.file_arg("thumbnail", file->get()->fileName, file->get()->mimeType, file->get()->data);
    } else {
      auto& str = std::get<std::string>(thumbnail);
      if (!str.empty())
        body.arg("thumbnail", str);
    }
    fillbody(body);
    json = $POST_REQUEST;
  } else {
    default_body_t body;
    body.arg("animation", std::get<std::string>(animation));
    auto& str = *std::get_if<1>(&thumbnail);
    if (!str.empty())
      body.arg("thumbnail", str);
    fillbody(body);
    json = $POST_REQUEST;
  }
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::sendVoice(int_or_str chatId, thumbnail_t voice, const std::string& caption,
                                      std::int32_t duration, ReplyParameters::Ptr replyParameters,
                                      GenericReply::Ptr replyMarkup, const std::string& parseMode,
                                      bool disableNotification,
                                      const std::vector<MessageEntity::Ptr>& captionEntities,
                                      std::int32_t messageThreadId, bool protectContent,
                                      const std::string& businessConnectionId) const {
  boost::property_tree::ptree json;

  auto fillbody = [&](auto& body) {
    if (!businessConnectionId.empty())
      body.arg("business_connection_id", businessConnectionId);
    body.arg("chat_id", chatId);
    if (messageThreadId != 0)
      body.arg("message_thread_id", messageThreadId);
    if (!caption.empty())
      body.arg("caption", caption);
    if (!parseMode.empty())
      body.arg("parse_mode", parseMode);
    if (!captionEntities.empty()) {
      body.arg("caption_entities",
               _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, captionEntities));
    }
    if (duration)
      body.arg("duration", duration);
    if (disableNotification)
      body.arg("disable_notification", disableNotification);
    if (protectContent)
      body.arg("protect_content", protectContent);
    if (replyParameters)
      body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
    if (replyMarkup)
      body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  };

  if (auto* file = std::get_if<InputFile::Ptr>(&voice)) {  // InputFile::Ptr
    application_multipart_form_data body;
    assert(*file);
    body.file_arg("voice", file->get()->fileName, file->get()->mimeType, file->get()->data);
    fillbody(body);
    json = $POST_REQUEST;
  } else {
    default_body_t body;
    body.arg("voice", std::get<std::string>(voice));
    fillbody(body);
    json = $POST_REQUEST;
  }
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::sendVideoNote(int_or_str chatId, thumbnail_t videoNote,
                                          ReplyParameters::Ptr replyParameters, bool disableNotification,
                                          std::int32_t duration, std::int32_t length, thumbnail_t thumbnail,
                                          GenericReply::Ptr replyMarkup, std::int32_t messageThreadId,
                                          bool protectContent,
                                          const std::string& businessConnectionId) const {
  boost::property_tree::ptree json;

  auto fillbody = [&](auto& body) {
    if (!businessConnectionId.empty())
      body.arg("business_connection_id", businessConnectionId);
    body.arg("chat_id", chatId);
    if (messageThreadId != 0)
      body.arg("message_thread_id", messageThreadId);
    if (duration)
      body.arg("duration", duration);
    if (length)
      body.arg("length", length);
    if (disableNotification)
      body.arg("disable_notification", disableNotification);
    if (protectContent)
      body.arg("protect_content", protectContent);
    if (replyParameters)
      body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
    if (replyMarkup)
      body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  };

  if (videoNote.index() == 0 || thumbnail.index() == 0) {  // InputFile::Ptr
    application_multipart_form_data body;
    if (auto* file = std::get_if<InputFile::Ptr>(&videoNote)) {
      assert(*file);
      body.file_arg("video_note", file->get()->fileName, file->get()->mimeType, file->get()->data);
    } else {
      body.arg("video_note", std::get<std::string>(videoNote));
    }
    if (auto* file = std::get_if<InputFile::Ptr>(&thumbnail)) {
      assert(*file);
      body.file_arg("thumbnail", file->get()->fileName, file->get()->mimeType, file->get()->data);
    } else {
      auto& str = std::get<std::string>(thumbnail);
      if (!str.empty())
        body.arg("thumbnail", str);
    }
    fillbody(body);
    json = $POST_REQUEST;
  } else {
    default_body_t body;
    body.arg("video_note", std::get<std::string>(videoNote));
    auto& str = *std::get_if<1>(&thumbnail);
    if (!str.empty())
      body.arg("thumbnail", str);
    fillbody(body);
    json = $POST_REQUEST;
  }
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<std::vector<Message::Ptr>> Api::sendMediaGroup(int_or_str chatId,
                                                        const std::vector<InputMedia::Ptr>& media,
                                                        bool disableNotification,
                                                        ReplyParameters::Ptr replyParameters,
                                                        std::int32_t messageThreadId, bool protectContent,
                                                        const std::string& businessConnectionId) const {
  default_body_t body;

  if (!businessConnectionId.empty())
    body.arg("business_connection_id", businessConnectionId);
  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  body.arg("media", _tgTypeParser.parseArray<InputMedia>(&TgTypeParser::parseInputMedia, media));
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (replyParameters)
    body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetArray<Message>(&TgTypeParser::parseJsonAndGetMessage, json);
}

dd::task<Message::Ptr> Api::sendLocation(int_or_str chatId, float latitude, float longitude,
                                         std::int32_t livePeriod, ReplyParameters::Ptr replyParameters,
                                         GenericReply::Ptr replyMarkup, bool disableNotification,
                                         float horizontalAccuracy, std::int32_t heading,
                                         std::int32_t proximityAlertRadius, std::int32_t messageThreadId,
                                         bool protectContent, const std::string& businessConnectionId) const {
  default_body_t body;

  if (!businessConnectionId.empty())
    body.arg("business_connection_id", businessConnectionId);
  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  body.arg("latitude", latitude);
  body.arg("longitude", longitude);
  if (horizontalAccuracy)
    body.arg("horizontal_accuracy", horizontalAccuracy);
  if (livePeriod)
    body.arg("live_period", std::max(60, std::min(86400, livePeriod)));
  if (heading)
    body.arg("heading", std::max(1, std::min(360, heading)));
  if (proximityAlertRadius)
    body.arg("proximity_alert_radius", std::max(1, std::min(100000, proximityAlertRadius)));
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (replyParameters)
    body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}
// TODO floats?.. use to_chars?
dd::task<Message::Ptr> Api::editMessageLiveLocation(float latitude, float longitude, int_or_str chatId,
                                                    std::int32_t messageId,
                                                    const std::string& inlineMessageId,
                                                    InlineKeyboardMarkup::Ptr replyMarkup,
                                                    float horizontalAccuracy, std::int32_t heading,
                                                    std::int32_t proximityAlertRadius) const {
  default_body_t body;

  if (!is_empty_chatid(chatId))
    body.arg("chat_id", chatId);
  if (messageId)
    body.arg("message_id", messageId);
  if (!inlineMessageId.empty())
    body.arg("inline_message_id", inlineMessageId);
  body.arg("latitude", latitude);
  body.arg("longitude", longitude);
  if (horizontalAccuracy)
    body.arg("horizontal_accuracy", horizontalAccuracy);
  if (heading)
    body.arg("heading", heading);
  if (proximityAlertRadius)
    body.arg("proximity_alert_radius", proximityAlertRadius);
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseInlineKeyboardMarkup(replyMarkup));

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::stopMessageLiveLocation(int_or_str chatId, std::int32_t messageId,
                                                    const std::string& inlineMessageId,
                                                    InlineKeyboardMarkup::Ptr replyMarkup) const {
  default_body_t body;

  if (!is_empty_chatid(chatId))
    body.arg("chat_id", chatId);
  if (messageId)
    body.arg("message_id", messageId);
  if (!inlineMessageId.empty())
    body.arg("inline_message_id", inlineMessageId);
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseInlineKeyboardMarkup(replyMarkup));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::sendVenue(int_or_str chatId, float latitude, float longitude,
                                      const std::string& title, const std::string& address,
                                      const std::string& foursquareId, const std::string& foursquareType,
                                      bool disableNotification, ReplyParameters::Ptr replyParameters,
                                      GenericReply::Ptr replyMarkup, const std::string& googlePlaceId,
                                      const std::string& googlePlaceType, std::int32_t messageThreadId,
                                      bool protectContent, const std::string& businessConnectionId) const {
  default_body_t body;

  if (!businessConnectionId.empty())
    body.arg("business_connection_id", businessConnectionId);
  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  body.arg("latitude", latitude);
  body.arg("longitude", longitude);
  body.arg("title", title);
  body.arg("address", address);
  if (!foursquareId.empty())
    body.arg("foursquare_id", foursquareId);
  if (!foursquareType.empty())
    body.arg("foursquare_type", foursquareType);
  if (!googlePlaceId.empty())
    body.arg("google_place_id", googlePlaceId);
  if (!googlePlaceType.empty())
    body.arg("google_place_type", googlePlaceType);
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (replyParameters)
    body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::sendContact(int_or_str chatId, const std::string& phoneNumber,
                                        const std::string& firstName, const std::string& lastName,
                                        const std::string& vcard, bool disableNotification,
                                        ReplyParameters::Ptr replyParameters, GenericReply::Ptr replyMarkup,
                                        std::int32_t messageThreadId, bool protectContent,
                                        const std::string& businessConnectionId) const {
  default_body_t body;

  if (!businessConnectionId.empty())
    body.arg("business_connection_id", businessConnectionId);
  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  body.arg("phone_number", phoneNumber);
  body.arg("first_name", firstName);
  if (!lastName.empty())
    body.arg("last_name", lastName);
  if (!vcard.empty())
    body.arg("vcard", vcard);
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (replyParameters)
    body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::sendPoll(int_or_str chatId, const std::string& question,
                                     const std::vector<std::string>& options, bool disableNotification,
                                     ReplyParameters::Ptr replyParameters, GenericReply::Ptr replyMarkup,
                                     bool isAnonymous, const std::string& type, bool allowsMultipleAnswers,
                                     std::int32_t correctOptionId, const std::string& explanation,
                                     const std::string& explanationParseMode,
                                     const std::vector<MessageEntity::Ptr>& explanationEntities,
                                     std::int32_t openPeriod, std::int32_t closeDate, bool isClosed,
                                     std::int32_t messageThreadId, bool protectContent,
                                     const std::string& businessConnectionId) const {
  default_body_t body;

  if (!businessConnectionId.empty())
    body.arg("business_connection_id", businessConnectionId);
  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  body.arg("question", question);
  body.arg("options", options);
  if (!isAnonymous)
    body.arg("is_anonymous", isAnonymous);
  if (!type.empty())
    body.arg("type", type);
  if (allowsMultipleAnswers)
    body.arg("allows_multiple_answers", allowsMultipleAnswers);
  if (correctOptionId != -1)
    body.arg("correct_option_id", correctOptionId);
  if (!explanation.empty())
    body.arg("explanation", explanation);
  if (!explanationParseMode.empty())
    body.arg("explanation_parse_mode", explanationParseMode);
  if (!explanationEntities.empty()) {
    body.arg("explanation_entities",
             _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, explanationEntities));
  }
  if (openPeriod != 0)
    body.arg("open_period", openPeriod);
  if (closeDate != 0)
    body.arg("close_date", closeDate);
  if (isClosed)
    body.arg("is_closed", isClosed);
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (replyParameters)
    body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::sendDice(int_or_str chatId, bool disableNotification,
                                     ReplyParameters::Ptr replyParameters, GenericReply::Ptr replyMarkup,
                                     const std::string& emoji, std::int32_t messageThreadId,
                                     bool protectContent, const std::string& businessConnectionId) const {
  default_body_t body;

  if (!businessConnectionId.empty())
    body.arg("business_connection_id", businessConnectionId);
  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  if (!emoji.empty())
    body.arg("emoji", emoji);
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (replyParameters)
    body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<bool> Api::setMessageReaction(int_or_str chatId, std::int32_t messageId,
                                       const std::vector<ReactionType::Ptr>& reaction, bool isBig) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("message_id", messageId);
  if (!reaction.empty()) {
    body.arg("reaction", _tgTypeParser.parseArray<ReactionType>(&TgTypeParser::parseReactionType, reaction));
  }
  if (isBig)
    body.arg("is_big", isBig);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::sendChatAction(std::int64_t chatId, const std::string& action,
                                   std::int32_t messageThreadId,
                                   const std::string& businessConnectionId) const {
  default_body_t body;

  if (!businessConnectionId.empty())
    body.arg("business_connection_id", businessConnectionId);
  body.arg("chat_id", chatId);
  body.arg("action", action);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<UserProfilePhotos::Ptr> Api::getUserProfilePhotos(std::int64_t userId, std::int32_t offset,
                                                           std::int32_t limit) const {
  default_body_t body;

  body.arg("user_id", userId);
  if (offset != 0)
    body.arg("offset", offset);
  if (limit != 100)
    body.arg("limit", std::max(1, std::min(100, limit)));

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetUserProfilePhotos(json);
}

dd::task<File::Ptr> Api::getFile(const std::string& fileId) const {
  default_body_t body;

  body.arg("file_id", fileId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetFile(json);
}

dd::task<bool> Api::banChatMember(int_or_str chatId, std::int64_t userId, std::int32_t untilDate,
                                  bool revokeMessages) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("user_id", userId);
  if (untilDate != 0)
    body.arg("until_date", untilDate);
  if (revokeMessages)
    body.arg("revoke_messages", revokeMessages);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::unbanChatMember(int_or_str chatId, std::int64_t userId, bool onlyIfBanned) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("user_id", userId);
  if (onlyIfBanned)
    body.arg("only_if_banned", onlyIfBanned);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::restrictChatMember(int_or_str chatId, std::int64_t userId,
                                       tgbm::ChatPermissions::Ptr permissions, std::uint32_t untilDate,
                                       bool useIndependentChatPermissions) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("user_id", userId);
  body.arg("permissions", _tgTypeParser.parseChatPermissions(permissions));
  if (useIndependentChatPermissions)
    body.arg("use_independent_chat_permissions", useIndependentChatPermissions);
  if (untilDate != 0)
    body.arg("until_date", untilDate);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::promoteChatMember(int_or_str chatId, std::int64_t userId, bool canChangeInfo,
                                      bool canPostMessages, bool canEditMessages, bool canDeleteMessages,
                                      bool canInviteUsers, bool canPinMessages, bool canPromoteMembers,
                                      bool isAnonymous, bool canManageChat, bool canManageVideoChats,
                                      bool canRestrictMembers, bool canManageTopics, bool canPostStories,
                                      bool canEditStories, bool canDeleteStories) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("user_id", userId);
  if (isAnonymous)
    body.arg("is_anonymous", isAnonymous);
  if (canManageChat)
    body.arg("can_manage_chat", canManageChat);
  if (canPostMessages)
    body.arg("can_post_messages", canPostMessages);
  if (canEditMessages)
    body.arg("can_edit_messages", canEditMessages);
  if (canDeleteMessages)
    body.arg("can_delete_messages", canDeleteMessages);
  if (canPostStories)
    body.arg("can_post_stories", canPostStories);
  if (canEditStories)
    body.arg("can_edit_stories", canEditStories);
  if (canDeleteStories)
    body.arg("can_delete_stories", canDeleteStories);
  if (canManageVideoChats)
    body.arg("can_manage_video_chats", canManageVideoChats);
  if (canRestrictMembers)
    body.arg("can_restrict_members", canRestrictMembers);
  if (canPromoteMembers)
    body.arg("can_promote_members", canPromoteMembers);
  if (canChangeInfo)
    body.arg("can_change_info", canChangeInfo);
  if (canInviteUsers)
    body.arg("can_invite_users", canInviteUsers);
  if (canPinMessages)
    body.arg("can_pin_messages", canPinMessages);
  if (canManageTopics)
    body.arg("can_manage_topics", canManageTopics);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setChatAdministratorCustomTitle(int_or_str chatId, std::int64_t userId,
                                                    const std::string& customTitle) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("user_id", userId);
  body.arg("custom_title", customTitle);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::banChatSenderChat(int_or_str chatId, std::int64_t senderChatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("sender_chat_id", senderChatId);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::unbanChatSenderChat(int_or_str chatId, std::int64_t senderChatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("sender_chat_id", senderChatId);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setChatPermissions(int_or_str chatId, ChatPermissions::Ptr permissions,
                                       bool useIndependentChatPermissions) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("permissions", _tgTypeParser.parseChatPermissions(permissions));
  if (useIndependentChatPermissions)
    body.arg("use_independent_chat_permissions", useIndependentChatPermissions);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<std::string> Api::exportChatInviteLink(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get("", "");
}

dd::task<ChatInviteLink::Ptr> Api::createChatInviteLink(int_or_str chatId, std::int32_t expireDate,
                                                        std::int32_t memberLimit, const std::string& name,
                                                        bool createsJoinRequest) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  if (!name.empty())
    body.arg("name", name);
  if (expireDate != 0)
    body.arg("expire_date", expireDate);
  if (memberLimit != 0)
    body.arg("member_limit", memberLimit);
  if (createsJoinRequest)
    body.arg("createsJoinRequest", createsJoinRequest);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetChatInviteLink(json);
}

dd::task<ChatInviteLink::Ptr> Api::editChatInviteLink(int_or_str chatId, const std::string& inviteLink,
                                                      std::int32_t expireDate, std::int32_t memberLimit,
                                                      const std::string& name,
                                                      bool createsJoinRequest) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("invite_link", inviteLink);
  if (!name.empty())
    body.arg("name", name);
  if (expireDate != 0)
    body.arg("expire_date", expireDate);
  if (memberLimit != 0)
    body.arg("member_limit", memberLimit);
  if (createsJoinRequest)
    body.arg("createsJoinRequest", createsJoinRequest);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetChatInviteLink(json);
}

dd::task<ChatInviteLink::Ptr> Api::revokeChatInviteLink(int_or_str chatId,
                                                        const std::string& inviteLink) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("invite_link", inviteLink);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetChatInviteLink(json);
}

dd::task<bool> Api::approveChatJoinRequest(int_or_str chatId, std::int64_t userId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("user_id", userId);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::declineChatJoinRequest(int_or_str chatId, std::int64_t userId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("user_id", userId);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setChatPhoto(int_or_str chatId, const InputFile::Ptr photo) const {
  application_multipart_form_data body;

  body.arg("chat_id", chatId);
  body.file_arg("photo", photo->fileName, photo->mimeType, photo->data);

  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::deleteChatPhoto(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setChatTitle(int_or_str chatId, const std::string& title) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("title", title);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setChatDescription(int_or_str chatId, const std::string& description) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  if (!description.empty())
    body.arg("description", description);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::pinChatMessage(int_or_str chatId, std::int32_t messageId,
                                   bool disableNotification) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("message_id", messageId);
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::unpinChatMessage(int_or_str chatId, std::int32_t messageId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  if (messageId != 0)
    body.arg("message_id", messageId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::unpinAllChatMessages(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::leaveChat(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<Chat::Ptr> Api::getChat(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetChat(json);
}

dd::task<std::vector<ChatMember::Ptr>> Api::getChatAdministrators(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetArray<ChatMember>(&TgTypeParser::parseJsonAndGetChatMember, json);
}

dd::task<int32_t> Api::getChatMemberCount(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<int32_t>("", 0);
}

dd::task<ChatMember::Ptr> Api::getChatMember(int_or_str chatId, std::int64_t userId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("user_id", userId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetChatMember(json);
}

dd::task<bool> Api::setChatStickerSet(int_or_str chatId, const std::string& stickerSetName) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("sticker_set_name	", stickerSetName);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::deleteChatStickerSet(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<std::vector<Sticker::Ptr>> Api::getForumTopicIconStickers() const {
  boost::property_tree::ptree json = $GET_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetArray<Sticker>(&TgTypeParser::parseJsonAndGetSticker, json);
}

dd::task<ForumTopic::Ptr> Api::createForumTopic(int_or_str chatId, const std::string& name,
                                                std::int32_t iconColor,
                                                const std::string& iconCustomEmojiId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("name", name);
  if (iconColor != 0)
    body.arg("icon_color", iconColor);
  if (!iconCustomEmojiId.empty())
    body.arg("icon_custom_emoji_id", iconCustomEmojiId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetForumTopic(json);
}

dd::task<bool> Api::editForumTopic(int_or_str chatId, std::int32_t messageThreadId, const std::string& name,
                                   int_or_str iconCustomEmojiId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("message_thread_id", messageThreadId);
  if (!name.empty())
    body.arg("name", name);
  if (!is_empty_chatid(iconCustomEmojiId))
    body.arg("icon_custom_emoji_id", iconCustomEmojiId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::closeForumTopic(int_or_str chatId, std::int32_t messageThreadId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("message_thread_id", messageThreadId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::reopenForumTopic(int_or_str chatId, std::int32_t messageThreadId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("message_thread_id", messageThreadId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::deleteForumTopic(int_or_str chatId, std::int32_t messageThreadId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("message_thread_id", messageThreadId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::unpinAllForumTopicMessages(int_or_str chatId, std::int32_t messageThreadId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("message_thread_id", messageThreadId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::editGeneralForumTopic(int_or_str chatId, std::string name) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("name", name);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::closeGeneralForumTopic(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::reopenGeneralForumTopic(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::hideGeneralForumTopic(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::unhideGeneralForumTopic(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::unpinAllGeneralForumTopicMessages(int_or_str chatId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::answerCallbackQuery(const std::string& callbackQueryId, const std::string& text,
                                        bool showAlert, const std::string& url,
                                        std::int32_t cacheTime) const {
  default_body_t body;

  body.arg("callback_query_id", callbackQueryId);
  if (!text.empty())
    body.arg("text", text);
  if (showAlert)
    body.arg("show_alert", showAlert);
  if (!url.empty())
    body.arg("url", url);
  if (cacheTime)
    body.arg("cache_time", cacheTime);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<UserChatBoosts::Ptr> Api::getUserChatBoosts(int_or_str chatId, std::int32_t userId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("user_id", userId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetUserChatBoosts(json);
}

dd::task<BusinessConnection::Ptr> Api::getBusinessConnection(const std::string& businessConnectionId) const {
  default_body_t body;

  body.arg("business_connection_id", businessConnectionId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetBusinessConnection(json);
}

dd::task<bool> Api::setMyCommands(const std::vector<BotCommand::Ptr>& commands, BotCommandScope::Ptr scope,
                                  const std::string& languageCode) const {
  default_body_t body;

  body.arg("commands", _tgTypeParser.parseArray<BotCommand>(&TgTypeParser::parseBotCommand, commands));
  if (scope)
    body.arg("scope", _tgTypeParser.parseBotCommandScope(scope));
  if (!languageCode.empty())
    body.arg("language_code", languageCode);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::deleteMyCommands(BotCommandScope::Ptr scope, const std::string& languageCode) const {
  default_body_t body;

  if (scope)
    body.arg("scope", _tgTypeParser.parseBotCommandScope(scope));
  if (!languageCode.empty())
    body.arg("language_code", languageCode);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<std::vector<BotCommand::Ptr>> Api::getMyCommands(BotCommandScope::Ptr scope,
                                                          const std::string& languageCode) const {
  default_body_t body;

  if (scope)
    body.arg("scope", _tgTypeParser.parseBotCommandScope(scope));
  if (!languageCode.empty())
    body.arg("language_code", languageCode);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetArray<BotCommand>(&TgTypeParser::parseJsonAndGetBotCommand, json);
}

dd::task<bool> Api::setMyName(const std::string& name, const std::string& languageCode) const {
  default_body_t body;

  if (!name.empty())
    body.arg("name", name);
  if (!languageCode.empty())
    body.arg("language_code", languageCode);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<BotName::Ptr> Api::getMyName(const std::string& languageCode) const {
  default_body_t body;

  if (!languageCode.empty())
    body.arg("language_code", languageCode);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetBotName(json);
}

dd::task<bool> Api::setMyDescription(const std::string& description, const std::string& languageCode) const {
  default_body_t body;

  if (!description.empty())
    body.arg("description", description);
  if (!languageCode.empty())
    body.arg("language_code", languageCode);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<BotDescription::Ptr> Api::getMyDescription(const std::string& languageCode) const {
  default_body_t body;

  if (!languageCode.empty())
    body.arg("language_code", languageCode);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetBotDescription(json);
}

dd::task<bool> Api::setMyShortDescription(const std::string& shortDescription,
                                          const std::string& languageCode) const {
  default_body_t body;

  if (!shortDescription.empty())
    body.arg("short_description", shortDescription);
  if (!languageCode.empty())
    body.arg("language_code", languageCode);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<BotShortDescription::Ptr> Api::getMyShortDescription(const std::string& languageCode) const {
  default_body_t body;

  if (!languageCode.empty())
    body.arg("language_code", languageCode);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetBotShortDescription(json);
}

dd::task<bool> Api::setChatMenuButton(std::int64_t chatId, MenuButton::Ptr menuButton) const {
  default_body_t body;

  if (chatId != 0)
    body.arg("chat_id", chatId);
  if (menuButton)
    body.arg("menu_button", _tgTypeParser.parseMenuButton(menuButton));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<MenuButton::Ptr> Api::getChatMenuButton(std::int64_t chatId) const {
  default_body_t body;

  if (chatId != 0)
    body.arg("chat_id", chatId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMenuButton(json);
}

dd::task<bool> Api::setMyDefaultAdministratorRights(ChatAdministratorRights::Ptr rights,
                                                    bool forChannels) const {
  default_body_t body;

  if (rights)
    body.arg("rights", _tgTypeParser.parseChatAdministratorRights(rights));
  if (forChannels)
    body.arg("for_channels", forChannels);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<ChatAdministratorRights::Ptr> Api::getMyDefaultAdministratorRights(bool forChannels) const {
  default_body_t body;

  if (forChannels)
    body.arg("for_channels", forChannels);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetChatAdministratorRights(json);
}

dd::task<Message::Ptr> Api::editMessageText(const std::string& text, int_or_str chatId,
                                            std::int32_t messageId, const std::string& inlineMessageId,
                                            const std::string& parseMode,
                                            LinkPreviewOptions::Ptr linkPreviewOptions,
                                            InlineKeyboardMarkup::Ptr replyMarkup,
                                            const std::vector<MessageEntity::Ptr>& entities) const {
  default_body_t body;

  if (!is_empty_chatid(chatId))
    body.arg("chat_id", chatId);
  if (messageId)
    body.arg("message_id", messageId);
  if (!inlineMessageId.empty())
    body.arg("inline_message_id", inlineMessageId);
  body.arg("text", text);
  if (!parseMode.empty())
    body.arg("parse_mode", parseMode);
  if (!entities.empty()) {
    body.arg("entities",
             _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, entities));
  }
  if (linkPreviewOptions)
    body.arg("link_preview_options", _tgTypeParser.parseLinkPreviewOptions(linkPreviewOptions));
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseInlineKeyboardMarkup(replyMarkup));

  boost::property_tree::ptree json = $POST_REQUEST;
  if (!json.get_child_optional("message_id"))
    co_return nullptr;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::editMessageCaption(int_or_str chatId, std::int32_t messageId,
                                               const std::string& caption, const std::string& inlineMessageId,
                                               GenericReply::Ptr replyMarkup, const std::string& parseMode,
                                               const std::vector<MessageEntity::Ptr>& captionEntities) const {
  default_body_t body;
  if (!is_empty_chatid(chatId))
    body.arg("chat_id", chatId);

  if (messageId)
    body.arg("message_id", messageId);
  if (!inlineMessageId.empty())
    body.arg("inline_message_id", inlineMessageId);
  if (!caption.empty())
    body.arg("caption", caption);
  if (!parseMode.empty())
    body.arg("parse_mode", parseMode);
  if (!captionEntities.empty()) {
    body.arg("caption_entities",
             _tgTypeParser.parseArray<MessageEntity>(&TgTypeParser::parseMessageEntity, captionEntities));
  }
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  boost::property_tree::ptree json = $POST_REQUEST;
  if (!json.get_child_optional("message_id"))
    co_return nullptr;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::editMessageMedia(InputMedia::Ptr media, int_or_str chatId, std::int32_t messageId,
                                             const std::string& inlineMessageId,
                                             GenericReply::Ptr replyMarkup) const {
  default_body_t body;

  if (!is_empty_chatid(chatId))
    body.arg("chat_id", chatId);

  body.arg("media", _tgTypeParser.parseInputMedia(media));
  if (messageId)
    body.arg("message_id", messageId);
  if (!inlineMessageId.empty())
    body.arg("inline_message_id", inlineMessageId);
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  boost::property_tree::ptree json = $POST_REQUEST;
  if (!json.get_child_optional("message_id"))
    co_return nullptr;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::editMessageReplyMarkup(int_or_str chatId, std::int32_t messageId,
                                                   const std::string& inlineMessageId,
                                                   const GenericReply::Ptr replyMarkup) const {
  default_body_t body;

  if (!is_empty_chatid(chatId))
    body.arg("chat_id", chatId);
  if (messageId)
    body.arg("message_id", messageId);
  if (!inlineMessageId.empty())
    body.arg("inline_message_id", inlineMessageId);
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));

  boost::property_tree::ptree json = $POST_REQUEST;
  if (!json.get_child_optional("message_id"))
    co_return nullptr;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Poll::Ptr> Api::stopPoll(int_or_str chatId, std::int64_t messageId,
                                  const InlineKeyboardMarkup::Ptr replyMarkup) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("message_id", messageId);
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetPoll(json);
}

dd::task<bool> Api::deleteMessage(int_or_str chatId, std::int32_t messageId) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  body.arg("message_id", messageId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::deleteMessages(int_or_str chatId, const std::vector<std::int32_t>& messageIds) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  if (!messageIds.empty())
    body.arg("message_ids", messageIds);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<Message::Ptr> Api::sendSticker(int_or_str chatId, thumbnail_t sticker,
                                        ReplyParameters::Ptr replyParameters, GenericReply::Ptr replyMarkup,
                                        bool disableNotification, std::int32_t messageThreadId,
                                        bool protectContent, const std::string& emoji,
                                        const std::string& businessConnectionId) const {
  auto fillbody = [&](auto& body) {
    if (!businessConnectionId.empty())
      body.arg("business_connection_id", businessConnectionId);
    body.arg("chat_id", chatId);
    if (messageThreadId != 0)
      body.arg("message_thread_id", messageThreadId);
    if (!emoji.empty())
      body.arg("emoji", emoji);
    if (disableNotification)
      body.arg("disable_notification", disableNotification);
    if (protectContent)
      body.arg("protect_content", protectContent);
    if (replyParameters)
      body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
    if (replyMarkup)
      body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  };

  boost::property_tree::ptree json;

  if (auto* file = std::get_if<InputFile::Ptr>(&sticker)) {
    application_multipart_form_data body;
    assert(*file);
    body.file_arg("sticker", file->get()->fileName, file->get()->mimeType, file->get()->data);
    fillbody(body);
    json = $POST_REQUEST;
  } else {
    default_body_t body;
    body.arg("sticker", std::get<std::string>(sticker));
    fillbody(body);
    json = $POST_REQUEST;
  }
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<StickerSet::Ptr> Api::getStickerSet(const std::string& name) const {
  default_body_t body;

  body.arg("name", name);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetStickerSet(json);
}

dd::task<std::vector<Sticker::Ptr>> Api::getCustomEmojiStickers(
    const std::vector<std::string>& customEmojiIds) const {
  default_body_t body;
  // TODO check vector of strings correctly escapes in json
  body.arg("custom_emoji_ids", customEmojiIds);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetArray<Sticker>(&TgTypeParser::parseJsonAndGetSticker, json);
}

dd::task<File::Ptr> Api::uploadStickerFile(std::int64_t userId, InputFile::Ptr sticker,
                                           const std::string& stickerFormat) const {
  application_multipart_form_data body;

  body.arg("user_id", userId);
  body.file_arg("sticker", sticker->fileName, sticker->mimeType, sticker->data);
  body.arg("sticker_format", stickerFormat);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetFile(json);
}

dd::task<bool> Api::createNewStickerSet(std::int64_t userId, const std::string& name,
                                        const std::string& title,
                                        const std::vector<InputSticker::Ptr>& stickers,
                                        Sticker::Type stickerType, bool needsRepainting) const {
  default_body_t body;

  body.arg("user_id", userId);
  body.arg("name", name);
  body.arg("title", title);
  body.arg("stickers", _tgTypeParser.parseArray<InputSticker>(&TgTypeParser::parseInputSticker, stickers));
  switch (stickerType) {
    case Sticker::Type::Regular:
      body.arg("sticker_type", "regular");
      break;
    case Sticker::Type::Mask:
      body.arg("sticker_type", "mask");
      break;
    case Sticker::Type::CustomEmoji:
      body.arg("sticker_type", "custom_emoji");
      break;
  }
  if (needsRepainting)
    body.arg("needs_repainting", needsRepainting);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::addStickerToSet(std::int64_t userId, const std::string& name,
                                    InputSticker::Ptr sticker) const {
  default_body_t body;

  body.arg("user_id", userId);
  body.arg("name", name);
  body.arg("sticker", _tgTypeParser.parseInputSticker(sticker));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setStickerPositionInSet(const std::string& sticker, std::int32_t position) const {
  default_body_t body;

  body.arg("sticker", sticker);
  body.arg("position", position);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::deleteStickerFromSet(const std::string& sticker) const {
  default_body_t body;

  body.arg("sticker", sticker);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::replaceStickerInSet(std::int64_t userId, const std::string& name,
                                        const std::string& oldSticker, InputSticker::Ptr sticker) const {
  default_body_t body;

  body.arg("user_id", userId);
  body.arg("name", name);
  body.arg("old_sticker", oldSticker);
  body.arg("sticker", _tgTypeParser.parseInputSticker(sticker));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setStickerEmojiList(const std::string& sticker,
                                        const std::vector<std::string>& emojiList) const {
  default_body_t body;

  body.arg("sticker", sticker);
  body.arg("emoji_list", emojiList);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setStickerKeywords(const std::string& sticker,
                                       const std::vector<std::string>& keywords) const {
  default_body_t body;

  body.arg("sticker", sticker);
  if (!keywords.empty())
    body.arg("keywords", keywords);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setStickerMaskPosition(const std::string& sticker, MaskPosition::Ptr maskPosition) const {
  default_body_t body;

  body.arg("sticker", sticker);
  if (maskPosition)
    body.arg("mask_position", _tgTypeParser.parseMaskPosition(maskPosition));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setStickerSetTitle(const std::string& name, const std::string& title) const {
  default_body_t body;

  body.arg("name", name);
  body.arg("title", title);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setStickerSetThumbnail(const std::string& name, std::int64_t userId,
                                           const std::string& format, thumbnail_t thumbnail) const {
  boost::property_tree::ptree json;

  auto fillbody = [&](auto& body) {
    body.arg("name", name);
    body.arg("user_id", userId);
    body.arg("format", format);
  };

  if (auto* file = std::get_if<InputFile::Ptr>(&thumbnail)) {
    application_multipart_form_data body;
    if (auto& file_ptr = *file)
      body.file_arg("thumbnail", file_ptr->fileName, file_ptr->mimeType, file_ptr->data);
    fillbody(body);
    json = $POST_REQUEST;
  } else {
    default_body_t body;
    body.arg("thumbnail", std::get<std::string>(thumbnail));
    fillbody(body);
    json = $POST_REQUEST;
  }
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setCustomEmojiStickerSetThumbnail(const std::string& name,
                                                      const std::string& customEmojiId) const {
  default_body_t body;

  body.arg("name", name);
  if (!customEmojiId.empty())
    body.arg("custom_emoji_id", customEmojiId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::deleteStickerSet(const std::string& name) const {
  default_body_t body;

  body.arg("name", name);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::answerInlineQuery(const std::string& inlineQueryId,
                                      const std::vector<InlineQueryResult::Ptr>& results,
                                      std::int32_t cacheTime, bool isPersonal, const std::string& nextOffset,
                                      InlineQueryResultsButton::Ptr button) const {
  default_body_t body;

  body.arg("inline_query_id", inlineQueryId);
  body.arg("results",
           _tgTypeParser.parseArray<InlineQueryResult>(&TgTypeParser::parseInlineQueryResult, results));
  if (cacheTime != 300)
    body.arg("cache_time", cacheTime);
  if (isPersonal)
    body.arg("is_personal", isPersonal);
  if (!nextOffset.empty())
    body.arg("next_offset", nextOffset);
  if (button)
    body.arg("button", _tgTypeParser.parseInlineQueryResultsButton(button));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<SentWebAppMessage::Ptr> Api::answerWebAppQuery(const std::string& webAppQueryId,
                                                        InlineQueryResult::Ptr result) const {
  default_body_t body;

  body.arg("web_app_query_id", webAppQueryId);
  body.arg("result", _tgTypeParser.parseInlineQueryResult(result));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetSentWebAppMessage(json);
}

dd::task<Message::Ptr> Api::sendInvoice(
    int_or_str chatId, const std::string& title, const std::string& description, const std::string& payload,
    const std::string& providerToken, const std::string& currency,
    const std::vector<LabeledPrice::Ptr>& prices, const std::string& providerData,
    const std::string& photoUrl, std::int32_t photoSize, std::int32_t photoWidth, std::int32_t photoHeight,
    bool needName, bool needPhoneNumber, bool needEmail, bool needShippingAddress,
    bool sendPhoneNumberToProvider, bool sendEmailToProvider, bool isFlexible,
    ReplyParameters::Ptr replyParameters, GenericReply::Ptr replyMarkup, bool disableNotification,
    std::int32_t messageThreadId, std::int32_t maxTipAmount,
    const std::vector<std::int32_t>& suggestedTipAmounts, const std::string& startParameter,
    bool protectContent) const {
  default_body_t body;

  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  body.arg("title", title);
  body.arg("description", description);
  body.arg("payload", payload);
  body.arg("provider_token", providerToken);
  body.arg("currency", currency);
  body.arg("prices", _tgTypeParser.parseArray<LabeledPrice>(&TgTypeParser::parseLabeledPrice, prices));
  body.arg("max_tip_amount", maxTipAmount);
  if (!suggestedTipAmounts.empty())
    body.arg("suggested_tip_amounts", suggestedTipAmounts);
  if (!startParameter.empty())
    body.arg("start_parameter", startParameter);
  if (!providerData.empty())
    body.arg("provider_data", providerData);
  if (!photoUrl.empty())
    body.arg("photo_url", photoUrl);
  if (photoSize)
    body.arg("photo_size", photoSize);
  if (photoWidth)
    body.arg("photo_width", photoWidth);
  if (photoHeight)
    body.arg("photo_height", photoHeight);
  if (needName)
    body.arg("need_name", needName);
  if (needPhoneNumber)
    body.arg("need_phone_number", needPhoneNumber);
  if (needEmail)
    body.arg("need_email", needEmail);
  if (needShippingAddress)
    body.arg("need_shipping_address", needShippingAddress);
  if (sendPhoneNumberToProvider)
    body.arg("send_phone_number_to_provider", sendPhoneNumberToProvider);
  if (sendEmailToProvider)
    body.arg("send_email_to_provider", sendEmailToProvider);
  if (isFlexible)
    body.arg("is_flexible", isFlexible);
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (replyParameters)
    body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<std::string> Api::createInvoiceLink(
    const std::string& title, const std::string& description, const std::string& payload,
    const std::string& providerToken, const std::string& currency,
    const std::vector<LabeledPrice::Ptr>& prices, std::int32_t maxTipAmount,
    const std::vector<std::int32_t>& suggestedTipAmounts, const std::string& providerData,
    const std::string& photoUrl, std::int32_t photoSize, std::int32_t photoWidth, std::int32_t photoHeight,
    bool needName, bool needPhoneNumber, bool needEmail, bool needShippingAddress,
    bool sendPhoneNumberToProvider, bool sendEmailToProvider, bool isFlexible) const {
  default_body_t body;

  body.arg("title", title);
  body.arg("description", description);
  body.arg("payload", payload);
  body.arg("provider_token", providerToken);
  body.arg("currency", currency);
  body.arg("prices", _tgTypeParser.parseArray<LabeledPrice>(&TgTypeParser::parseLabeledPrice, prices));
  body.arg("max_tip_amount", maxTipAmount);
  if (!suggestedTipAmounts.empty())
    body.arg("suggested_tip_amounts", suggestedTipAmounts);
  if (!providerData.empty())
    body.arg("provider_data", providerData);
  if (!photoUrl.empty())
    body.arg("photo_url", photoUrl);
  if (photoSize)
    body.arg("photo_size", photoSize);
  if (photoWidth)
    body.arg("photo_width", photoWidth);
  if (photoHeight)
    body.arg("photo_height", photoHeight);
  if (needName)
    body.arg("need_name", needName);
  if (needPhoneNumber)
    body.arg("need_phone_number", needPhoneNumber);
  if (needEmail)
    body.arg("need_email", needEmail);
  if (needShippingAddress)
    body.arg("need_shipping_address", needShippingAddress);
  if (sendPhoneNumberToProvider)
    body.arg("send_phone_number_to_provider", sendPhoneNumberToProvider);
  if (sendEmailToProvider)
    body.arg("send_email_to_provider", sendEmailToProvider);
  if (isFlexible)
    body.arg("is_flexible", isFlexible);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<std::string>("", "");
}

dd::task<bool> Api::answerShippingQuery(const std::string& shippingQueryId, bool ok,
                                        const std::vector<ShippingOption::Ptr>& shippingOptions,
                                        const std::string& errorMessage) const {
  default_body_t body;

  body.arg("shipping_query_id", shippingQueryId);
  body.arg("ok", ok);
  if (!shippingOptions.empty()) {
    body.arg("shipping_options",
             _tgTypeParser.parseArray<ShippingOption>(&TgTypeParser::parseShippingOption, shippingOptions));
  }
  if (!errorMessage.empty())
    body.arg("error_message", errorMessage);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::answerPreCheckoutQuery(const std::string& preCheckoutQueryId, bool ok,
                                           const std::string& errorMessage) const {
  default_body_t body;

  body.arg("pre_checkout_query_id", preCheckoutQueryId);
  body.arg("ok", ok);
  if (!errorMessage.empty())
    body.arg("error_message", errorMessage);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<bool> Api::setPassportDataErrors(std::int64_t userId,
                                          const std::vector<PassportElementError::Ptr>& errors) const {
  default_body_t body;

  body.arg("user_id", userId);
  body.arg("errors",
           _tgTypeParser.parseArray<PassportElementError>(&TgTypeParser::parsePassportElementError, errors));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return json.get<bool>("", false);
}

dd::task<Message::Ptr> Api::sendGame(std::int64_t chatId, const std::string& gameShortName,
                                     ReplyParameters::Ptr replyParameters,
                                     InlineKeyboardMarkup::Ptr replyMarkup, bool disableNotification,
                                     std::int32_t messageThreadId, bool protectContent,
                                     const std::string& businessConnectionId) const {
  default_body_t body;

  if (!businessConnectionId.empty())
    body.arg("business_connection_id", businessConnectionId);
  body.arg("chat_id", chatId);
  if (messageThreadId != 0)
    body.arg("message_thread_id", messageThreadId);
  body.arg("game_short_name", gameShortName);
  if (disableNotification)
    body.arg("disable_notification", disableNotification);
  if (protectContent)
    body.arg("protect_content", protectContent);
  if (replyParameters)
    body.arg("reply_parameters", _tgTypeParser.parseReplyParameters(replyParameters));
  if (replyMarkup)
    body.arg("reply_markup", _tgTypeParser.parseGenericReply(replyMarkup));
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<Message::Ptr> Api::setGameScore(std::int64_t userId, std::int32_t score, bool force,
                                         bool disableEditMessage, std::int64_t chatId, std::int32_t messageId,
                                         const std::string& inlineMessageId) const {
  default_body_t body;

  body.arg("user_id", userId);
  body.arg("score", score);
  if (force)
    body.arg("force", force);
  if (disableEditMessage)
    body.arg("disable_edit_message", disableEditMessage);
  if (chatId)
    body.arg("chat_id", chatId);
  if (messageId)
    body.arg("message_id", messageId);
  if (!inlineMessageId.empty())
    body.arg("inline_message_id", inlineMessageId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetMessage(json);
}

dd::task<std::vector<GameHighScore::Ptr>> Api::getGameHighScores(std::int64_t userId, std::int64_t chatId,
                                                                 std::int32_t messageId,
                                                                 const std::string& inlineMessageId) const {
  default_body_t body;

  body.arg("user_id", userId);
  if (chatId)
    body.arg("chat_id", chatId);
  if (messageId)
    body.arg("message_id", messageId);
  if (!inlineMessageId.empty())
    body.arg("inline_message_id", inlineMessageId);
  boost::property_tree::ptree json = $POST_REQUEST;
  co_return _tgTypeParser.parseJsonAndGetArray<GameHighScore>(&TgTypeParser::parseJsonAndGetGameHighScore,
                                                              json);
}

// dd::task<std::vector<StarTransaction>> Api::getStarTransactions(std::int64_t offset,
//                                                                 std::int64_t limit) const {
//   default_body_t body;
//   if (offset != -1)
//     body.arg("offset", offset);
//   if (limit != 100)
//     body.arg("limit", limit);
//   boost::property_tree::ptree json = $POST_REQUEST;
//   // TODO better _tgTypeParser
//   co_return
//   _tgTypeParser.parseJsonAndGetArray<StarTransaction>(&TgTypeParser::parseJsonAndGetStarTransaction,
//                                                                 json);
// }
// dd::task<bool> Api::refundStarPayment(std::int64_t user_id, std::string telegram_payment_charge_id) const {
//}

// TODO understand why 'args' presented?? Что туда блин передавать то можно?
dd::task<http_response> Api::downloadFile(const std::string& filePath) {
  http_response rsp = co_await _httpClient.send_request(
      http_request{.path = fmt::format("/file/bot{}/{}", get_token(), filePath)}, duration_t::max());
  co_return rsp;
}

dd::task<bool> Api::blockedByUser(std::int64_t chatId) const {
  try {
    (void)co_await sendChatAction(chatId, "typing");
    co_return false;
  } catch (std::exception& e) {
    // TODO somehow check its working
    co_return std::string_view("Forbidden: bot was blocked by the user") == e.what();
  }
}

dd::task<boost::property_tree::ptree> Api::sendRequest(http_request request, duration_t timeout) const {
  // TODO parse http response
  http_response response = co_await _httpClient.send_request(std::move(request), timeout);
  if (tgbm::starts_with(response.body,
                        "<html>"))  // TODO проверить работоспособность этой ветки с неправильным токеном
    throw TGBM_TG_EXCEPTION(tg_errc::HtmlResponse,
                            "TGBM library have got html page instead of json response. "
                            "Maybe you entered wrong bot token or incorrect host");

  boost::property_tree::ptree result;
  try {
    // TODO better
    result = _tgTypeParser.parseJson(std::string((char*)response.body.data(), response.body.size()));
  } catch (boost::property_tree::ptree_error& e) {
    throw TGBM_TG_EXCEPTION(tg_errc::InvalidJson, "TGBM library can't parse json response. {}", e.what());
  }
  if (!result.get<bool>("ok", false)) {
    auto x = result.get_optional<bool>("ok");
    if (x && !*x) {
      LOG_ERR("json with error: {}", std::string_view((char*)response.body.data(), response.body.size()));
    } else {
      throw TGBM_TG_EXCEPTION(tg_errc(result.get<size_t>("error_code", 0u)), "{}",
                              result.get("description", ""));
    }
  }
  // TODO тоже копирование мда
  co_return result.get_child("result");
}

}  // namespace tgbm

#undef $METHOD
#undef $POST_REQUEST
#undef $GET_REQUEST
