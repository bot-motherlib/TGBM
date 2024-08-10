#include "tgbm/net/TgLongPoll.h"

#include "tgbm/Api.h"
#include "tgbm/Bot.h"
#include "tgbm/EventHandler.h"

#include <cstdint>
#include <memory>
#include <vector>
#include <utility>

namespace tgbm {

TgLongPoll::TgLongPoll(const Api* api, const EventHandler* eventHandler, std::int32_t limit,
                       std::int32_t timeout, std::shared_ptr<std::vector<std::string>> allowUpdates)
    : _api(api),
      _eventHandler(eventHandler),
      _limit(limit),
      _timeout(timeout),
      _allowUpdates(std::move(allowUpdates)) {
  const_cast<tgbm::HttpClient&>(_api->_httpClient)._timeout = _timeout + 5;
}

TgLongPoll::TgLongPoll(const Bot& bot, std::int32_t limit, std::int32_t timeout,
                       const std::shared_ptr<std::vector<std::string>>& allowUpdates)
    : TgLongPoll(&bot.getApi(), &bot.getEventHandler(), limit, timeout, allowUpdates) {
}

dd::task<void> TgLongPoll::start() {
  //// handle updates
  // for (Update::Ptr& item : _updates) {
  //   if (item->updateId >= _lastUpdateId) {
  //     _lastUpdateId = item->updateId + 1;
  //   }
  //   _eventHandler->handleUpdate(item);
  // }

  // confirm handled updates
  // TODO async (channel...)
  // TODO это очевидно на одном потоке всегда должно происходить...
  // почему то не обновляются апдейты, хм... странное что то
  //_updates
  auto updates = co_await _api->getUpdates(_lastUpdateId, _limit, _timeout, _allowUpdates);
  for (Update::Ptr& item : updates) {
    if (item->updateId >= _lastUpdateId) {
      _lastUpdateId = item->updateId + 1;
    }
    _eventHandler->handleUpdate(item);
  }
}

}  // namespace tgbm
