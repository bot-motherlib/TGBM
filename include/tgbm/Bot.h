#pragma once

#include "tgbm/Api.h"
#include "tgbm/EventHandler.h"
#include "tgbm/net/long_poll.hpp"

#include <memory>
#include <string>

namespace tgbm {

class EventBroadcaster;
struct http_client;

std::unique_ptr<http_client> default_http_client(std::string_view host);

// short cut for creating http client, api, updater(long pool or smth) and update visitor
struct Bot {
 private:
  // invariant: != nullptr
  std::unique_ptr<http_client> _client;
  Api _api;
  std::unique_ptr<EventBroadcaster> _eventBroadcaster;
  EventHandler _eventHandler;

 public:
  // uses default http client
  explicit Bot(std::string token, std::string host = "api.telegram.org");

  explicit Bot(std::string token, std::unique_ptr<http_client> client);

  std::string_view get_token() const noexcept KELCORO_LIFETIMEBOUND {
    return _api.get_token();
  }
  std::string_view get_host() const noexcept {
    return _client->get_host();
  }

  const Api& get_api() const {
    return _api;
  }
  http_client& get_client() const {
    return *_client;
  }

  // sets timeout to all api request
  void set_timeout(duration_t timeout) {
    _api.set_timeout(timeout);
  }

  duration_t get_timeout() const noexcept {
    return _api.get_timeout();
  }
  /**
   * @return Object which holds all event listeners.
   */
  EventBroadcaster& getEvents() {
    return *_eventBroadcaster;
  }

  // stops only after exception in getting or handling updates
  void run(std::chrono::seconds update_wait_timeout = std::chrono::seconds(10));

  void stop();

 private:
  dd::task<void> get_and_handle_updates(std::chrono::seconds update_wait_timeout);
};

}  // namespace tgbm
