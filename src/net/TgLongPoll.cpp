#include "tgbm/net/TgLongPoll.h"

#include "tgbm/Api.h"
#include "tgbm/Bot.h"

#include "tgbm/logger.h"

namespace tgbm {

dd::channel<Update::Ptr> long_poll(const Api* api, std::int32_t limit, std::chrono::seconds timeout,
                                   std::shared_ptr<std::vector<std::string>> allowUpdates) {
  // TODO один коннекшн и через него делать всё и всегда, не переиспользовать
  // confirms handled updates
  std::int32_t lastUpdateId = 0;
  std::vector<std::shared_ptr<tgbm::Update>> updates;
  (void)co_await api->deleteWebhook();  // getUpdates does not work until webHooks are deleted
  for (;;) {
    updates = co_await api->getUpdates(lastUpdateId, limit, timeout.count(), allowUpdates);
    LOG("[updates] getted {} updates", updates.size());
    for (Update::Ptr& item : updates) {
      if (item->updateId >= lastUpdateId)
        lastUpdateId = item->updateId + 1;
      co_yield std::move(item);
    }
  }
  LOG("[updates] long pool ended after stop request");
}

dd::channel<Update::Ptr> long_poll(const Bot& bot, std::int32_t limit, std::chrono::seconds timeout,
                                   std::shared_ptr<std::vector<std::string>> allowUpdates) {
  return long_poll(&bot.getApi(), limit, timeout, std::move(allowUpdates));
}

}  // namespace tgbm
