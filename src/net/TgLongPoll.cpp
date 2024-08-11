#include "tgbm/net/TgLongPoll.h"

#include "tgbm/Api.h"
#include "tgbm/Bot.h"
#include "tgbm/EventHandler.h"

#include <cstdint>
#include <memory>
#include <vector>
#include <utility>

#include "tgbm/logger.h"

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
  // TODO один коннекшн и через него делать всё и всегда, не переиспользовать
  if (started)
    throw 42;
  started = true;
  auto limit = _limit;
  auto timeout = _timeout;
  auto allowUpdates = _allowUpdates;
  // confirm handled updates
  std::int32_t lastUpdateId = 0;
  std::vector<std::shared_ptr<tgbm::Update>> updates;
  while (true) {
    try {
      updates = co_await _api->getUpdates(lastUpdateId, limit, timeout, allowUpdates);
    } catch (std::exception& e) {
      LOG_ERR("[updates] getUpdates exception: {}", e.what());
    } catch (...) {
      LOG_ERR("[updates] UNKNOWN EXCEPTION");
    }
    LOG("[updates] getted {} updates on {} thread", updates.size(),
        std::bit_cast<unsigned>(std::this_thread::get_id()));
    for (Update::Ptr& item : updates) {
      if (item->updateId >= lastUpdateId) {
        lastUpdateId = item->updateId + 1;
      }
      LOG("[updates] handle id #{}", lastUpdateId);
      // TODO нужно хендлер тоже чтоли захватывать, в общем однопоточно совершенно читать и обрабатывать
      // ивенты
      _eventHandler->handleUpdate(item);
    }
  }
}

}  // namespace tgbm
