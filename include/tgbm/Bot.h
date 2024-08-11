#pragma once

#include "tgbm/Api.h"
#include "tgbm/EventHandler.h"

#include <memory>
#include <string>
#include <utility>

namespace tgbm {

class EventBroadcaster;
class HttpClient;

/**
 * @brief This object holds other objects specific for this bot instance.
 *
 * @ingroup general
 */
class TGBM_API Bot {
 public:
  explicit Bot(std::string token, HttpClient& httpClient,
               const std::string& url = "https://api.telegram.org");

  /**
   * @return Token for accessing api.
   */
  inline const std::string& getToken() const {
    return _token;
  }

  /**
   * @return Object which can execute Telegram Bot API methods.
   */
  inline const Api& getApi() const {
    return _api;
  }

  /**
   * @return Object which holds all event listeners.
   */
  inline EventBroadcaster& getEvents() {
    return *_eventBroadcaster;
  }

  /**
   * @return Object which handles new update objects. Usually it's only needed for TgLongPoll,
   * TgWebhookLocalServer and TgWebhookTcpServer objects.
   */
  inline const EventHandler& getEventHandler() const {
    return _eventHandler;
  }

 private:
  const std::string _token;
  const Api _api;
  std::unique_ptr<EventBroadcaster> _eventBroadcaster;
  const EventHandler _eventHandler;
};

}  // namespace tgbm
