#include "tgbm/Bot.h"

#include "tgbm/EventBroadcaster.h"
#include "tgbm/net/http2_client.hpp"

#include <memory>
#include <string>

#include "tgbm/logger.h"
#include "tgbm/tools/scope_exit.h"

namespace tgbm {
// TODO any client
std::unique_ptr<http_client> default_http_client(std::string_view host) {
  return std::unique_ptr<http_client>(new http2_client(host));
}

Bot::Bot(std::string token, std::string host)
    : _client(default_http_client(host)),
      _api(token, *_client),
      _eventBroadcaster(new EventBroadcaster),
      _eventHandler(*_eventBroadcaster) {
}

Bot::Bot(std::string token, std::unique_ptr<http_client> httpClient)
    : _client(std::move(httpClient)),
      _api(token, *_client),
      // TODO rm эти штуки, заменить на update_visitor + updater типа того
      _eventBroadcaster(new EventBroadcaster),
      _eventHandler(*_eventBroadcaster) {
  assert(_client && "client must not be nullptr");
}

dd::task<void> Bot::get_and_handle_updates(std::chrono::seconds update_wait_timeout) {
  on_scope_exit {
    _client->stop();
  };
  try {
    co_foreach(Update::Ptr update,
               long_poll(get_api(), 100, update_wait_timeout, nullptr, /*confirm_before_handle=*/true)) {
      _eventHandler.handleUpdate(update);
    }
  } catch (std::exception& e) {
    LOG_ERR("Bot getUpdates ended with exception, its ignored, err: {}", e.what());
    LOG_ERR("getUpdates ended with exception, http client will be stopped, what: {}", e.what());
  } catch (...) {
    LOG_ERR("getUpdates ended with unknown exception, http client will be stopped");
  }
}

void Bot::run(std::chrono::seconds update_wait_timeout) {
  auto handle = get_and_handle_updates(update_wait_timeout).start_and_detach(/*stop_at_end=*/true);
  LOG_DEBUG("start bot");
  _client->run();
  std::exception_ptr e = handle.promise().exception;
  handle.destroy();
  if (e) {
    LOG("Bot stopped with exception in getUpdates");
    std::rethrow_exception(e);
  } else {
    LOG("Bot stopped without exception");
  }
}

void Bot::stop() {
  _client->stop();
}

}  // namespace tgbm
