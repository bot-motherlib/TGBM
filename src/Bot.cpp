#include "tgbm/Bot.h"

#include "tgbm/EventBroadcaster.h"

#include <memory>
#include <string>

namespace tgbm {

Bot::Bot(std::string token, HttpClient& httpClient, std::string url)
    : _api(token, httpClient, std::move(url)),
      _eventBroadcaster(new EventBroadcaster),
      _eventHandler(*_eventBroadcaster) {
}

}  // namespace tgbm
