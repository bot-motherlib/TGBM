#pragma once

#include <cstdint>
#include <memory>
#include <string>
// TODO разобраться откуда лезет #define YIeld и прикрыть эти хедеры (азио)
#ifdef _WIN32
#undef Yield
#endif
#include <kelcoro/channel.hpp>

#include "tgbm/types/Update.h"

namespace tgbm {
// TODO fwd declare and using value...
class Api;

// Note: getUpdates does not work until webHooks are deleted
// so use api.deleteWebhook() before long pool if you was using webHooks
//
// 'confirm_before_handle' may be used for case, when handling received update may broke bot
//      (segfaualt or just bot.stop()) so bot will not confirm handled updates after error,
//     and will get same update (with possibly same error) after restart.
//     'true' will confirm handling updates before handling, so bot will not get bad updates
//     after restart
dd::channel<Update::Ptr> long_poll(Api api, std::int32_t limit, std::chrono::seconds timeout,
                                   std::shared_ptr<std::vector<std::string>> allowUpdates = nullptr,
                                   bool confirm_before_handle = false);

}  // namespace tgbm
