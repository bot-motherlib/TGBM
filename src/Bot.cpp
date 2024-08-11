#include "tgbm/net/BoostHttpOnlySslClient.h"
#include "tgbm/Bot.h"

#include "tgbm/EventBroadcaster.h"

#include <memory>
#include <string>

namespace tgbm {

Bot::Bot(std::string token, HttpClient& httpClient, const std::string& url)
    : _token(std::move(token)),
      _api(_token, httpClient, url),
      _eventBroadcaster(std::make_unique<EventBroadcaster>()),
      _eventHandler(getEvents()) {
}

}  // namespace tgbm
