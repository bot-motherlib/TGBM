#pragma once

// TODO разобраться откуда лезет #define YIeld и прикрыть эти хедеры (азио)
#ifdef _WIN32
  #undef Yield
#endif
#include <kelcoro/channel.hpp>

#include "tgbm/api/types/all.hpp"
#include "tgbm/api/telegram.hpp"
#include "tgbm/api/arrayof.hpp"

namespace tgbm {

// Note: getUpdates does not work until webHooks are deleted
// so use api.deleteWebhook() before long pool if you was using webHooks
//
// 'drop_pending_updates' - ignores what first 'getUpdates' will return
// 'limit' must be in range [1, 100]
dd::channel<api::Update> long_poll(api::telegram api, std::int32_t limit = 100,
                                   std::chrono::seconds timeout = std::chrono::seconds(100),
                                   api::arrayof<api::String> allowed_updates = {},
                                   bool drop_pending_updates = false);

}  // namespace tgbm
