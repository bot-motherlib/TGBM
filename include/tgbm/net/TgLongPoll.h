#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
// TODO разобраться откуда лезет #define YIeld и прикрыть эти хедеры (азио)
#ifdef _WIN32
#undef Yield
#endif
#include <kelcoro/channel.hpp>

#include "tgbm/types/Update.h"

namespace tgbm {

class Bot;
class EventHandler;
class Api;

dd::channel<Update::Ptr> long_poll(const Api* api KELCORO_LIFETIMEBOUND,
                                   const EventHandler* eventHandler KELCORO_LIFETIMEBOUND,
                                   std::int32_t limit = 100,
                                   std::chrono::seconds timeout = std::chrono::seconds(10),
                                   std::shared_ptr<std::vector<std::string>> allowUpdates = nullptr);

dd::channel<Update::Ptr> long_poll(const Bot& bot, std::int32_t limit = 100,
                                   std::chrono::seconds timeout = std::chrono::seconds(10),
                                   std::shared_ptr<std::vector<std::string>> allowUpdates = nullptr);

}  // namespace tgbm
