#pragma once

#include "tgbm/Api.h"
#include "tgbm/export.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace tgbm {

class Bot;
class EventHandler;
/*
// TODO?? may be use to find biggest handled event id

template <typename T, typename Func>
T atomic_update(std::atomic<T>& atomic, Func updater) {
  T old_value = atomic.load();
  while (true) {
    // make a copy to to keep old_value unchanged
    const T new_value = updater(T{old_value});

    // don't mark cache line as dirty
    if (old_value == new_value)
      return old_value;

    if (atomic.compare_exchange_weak(old_value, new_value))
      return new_value;
  }
}

template <typename T>
T atomic_min(std::atomic<T>& atomic, T value) {
  return atomic_update(atomic, [value](T old_value) { return value < old_value ? value : old_value; });
}

template <typename T>
T atomic_max(std::atomic<T>& atomic, T value) {
  return atomic_update(atomic, [value](T old_value) { return old_value < value ? value : old_value; });
}
*/

/**
 * @brief This class handles long polling and updates parsing.
 *
 * @ingroup net
 */
class TGBM_API TgLongPoll {
 public:
  TgLongPoll(const Api* api, const EventHandler* eventHandler, std::int32_t limit, std::int32_t timeout,
             std::shared_ptr<std::vector<std::string>> allowUpdates);
  TgLongPoll(const Bot& bot, std::int32_t limit = 100, std::int32_t timeout = 10,
             const std::shared_ptr<std::vector<std::string>>& allowUpdates = nullptr);

  /**
   * @brief Starts long poll. After new update will come, this method will parse it and send to EventHandler
   * which invokes your listeners. Designed to be executed in a loop.
   */
  dd::task<void> start();

 private:
  const Api* _api;
  const EventHandler* _eventHandler;
  // TODO а эту переменную разные треды не юзают? Или ещё что-то, в общем не то обновляется
  // и ггвп
  std::int32_t _limit;
  std::int32_t _timeout;
  std::shared_ptr<std::vector<std::string>> _allowUpdates;
  bool started = false;  // TODO rm, checks if only 1 updater exist
};

}  // namespace tgbm
