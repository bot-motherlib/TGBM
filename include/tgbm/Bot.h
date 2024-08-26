#pragma once

#include "tgbm/Api.h"
#include "tgbm/EventHandler.h"
#include "tgbm/net/long_poll.hpp"

#include <memory>
#include <string>

namespace tgbm {

class EventBroadcaster;
class HttpClient;

std::unique_ptr<HttpClient> default_http_client(std::string host);

// short cut for creating http client, api, updater(long pool or smth) and update visitor
class Bot {
 public:
  // uses default http client
  explicit Bot(std::string token, std::string host = "api.telegram.org");

  explicit Bot(std::string token, std::unique_ptr<HttpClient> client, std::string host = "api.telegram.org");
  // TODO remove .get отсюда
  std::string_view get_token() const noexcept KELCORO_LIFETIMEBOUND {
    return _api.get_token();
  }
  const Api& get_api() const {
    return _api;
  }
  const HttpClient& get_client() const {
    return *_client;
  }

  // sets timeout to all api request
  void set_timeout(duration_t timeout) {
    _api.set_timeout(timeout);
  }

  /**
   * @return Object which holds all event listeners.
   */
  EventBroadcaster& getEvents() {
    return *_eventBroadcaster;
  }

  // stops only after exception in getting or handling updates
  void run(std::chrono::seconds update_wait_timeout = std::chrono::seconds(10));

 private:
  dd::task<void> get_and_handle_updates(std::chrono::seconds update_wait_timeout);

  // invariant: != nullptr
  std::unique_ptr<HttpClient> _client;
  Api _api;
  std::unique_ptr<EventBroadcaster> _eventBroadcaster;
  EventHandler _eventHandler;
};

}  // namespace tgbm
