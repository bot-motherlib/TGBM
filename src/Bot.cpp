#include "tgbm/Bot.h"

#include "tgbm/EventBroadcaster.h"
#include "tgbm/net/boost_http_client.h"

#include <memory>
#include <string>

namespace tgbm {

std::unique_ptr<HttpClient> default_http_client(std::string host) {
  return std::unique_ptr<HttpClient>(new boost_singlethread_client(std::move(host)));
}

Bot::Bot(std::string token, std::string host)
    : _client(default_http_client(host)),
      _api(token, *_client, std::move(host)),
      _eventBroadcaster(new EventBroadcaster),
      _eventHandler(*_eventBroadcaster) {
}

Bot::Bot(std::string token, std::unique_ptr<HttpClient> httpClient, std::string host)
    : _client(httpClient ? std::move(httpClient) : default_http_client(host)),
      _api(token, *_client, std::move(host)),
      // TODO rm эти штуки, заменить на update_visitor + updater типа того
      _eventBroadcaster(new EventBroadcaster),
      _eventHandler(*_eventBroadcaster) {
}

dd::task<void> Bot::get_and_handle_updates(std::chrono::seconds update_wait_timeout) {
  on_scope_exit {
    _client->stop();
  };
  co_foreach(Update::Ptr update, long_poll(get_api(), 100, update_wait_timeout)) {
    _eventHandler.handleUpdate(update);
  }
}

void Bot::run(std::chrono::seconds update_wait_timeout) {
  auto handle = get_and_handle_updates(update_wait_timeout).start_and_detach(/*stop_at_end=*/true);
  _client->run();
  std::exception_ptr e = handle.promise().exception;
  assert(e);
  handle.destroy();
  std::rethrow_exception(e);
}

}  // namespace tgbm
