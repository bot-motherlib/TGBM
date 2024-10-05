#include "tgbm/net/long_poll.hpp"

#include "tgbm/Api.h"

#include "tgbm/logger.h"

namespace tgbm {

static dd::channel<Update::Ptr> long_poll_with_preconfirm(
    Api api, std::int32_t limit, std::chrono::seconds timeout,
    std::shared_ptr<std::vector<std::string>> allowUpdates) {
  // confirms handled updates
  std::int32_t lastUpdateId = 0;
  for (;;) {
    std::vector<std::shared_ptr<tgbm::Update>> updates =
        co_await api.getUpdates(lastUpdateId, limit, timeout.count(), allowUpdates);
    if (!updates.empty()) {
      for (auto& u : updates) {
        if (u->updateId >= lastUpdateId)
          lastUpdateId = u->updateId + 1;
      }
      // mark updates as handled before handling them
      // ignores update, so its unhanled == will be returned next time
      (void)co_await api.getUpdates(lastUpdateId, 1, 0, allowUpdates);
    }
    LOG("[updates] getted {} updates", updates.size());
    for (Update::Ptr& item : updates)
      co_yield std::move(item);
  }
}

static dd::channel<Update::Ptr> long_poll_without_preconfirm(
    Api api, std::int32_t limit, std::chrono::seconds timeout,
    std::shared_ptr<std::vector<std::string>> allowUpdates) {
  // confirms handled updates
  std::int32_t lastUpdateId = 0;
  std::vector<std::shared_ptr<tgbm::Update>> updates;
  for (;;) {
    updates = co_await api.getUpdates(lastUpdateId, limit, timeout.count(), allowUpdates);
    LOG("[updates] getted {} updates", updates.size());
    for (Update::Ptr& item : updates) {
      if (item->updateId >= lastUpdateId)
        lastUpdateId = item->updateId + 1;
      co_yield std::move(item);
    }
  }
}

dd::channel<Update::Ptr> long_poll(Api api, std::int32_t limit, std::chrono::seconds timeout,
                                   std::shared_ptr<std::vector<std::string>> allowUpdates,
                                   bool confirm_before_handle) {
  if (confirm_before_handle)
    return long_poll_with_preconfirm(std::move(api), limit, timeout, std::move(allowUpdates));
  return long_poll_without_preconfirm(std::move(api), limit, timeout, std::move(allowUpdates));
}

}  // namespace tgbm
