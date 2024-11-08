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
// so api.deleteWebhook() is called before polling
dd::channel<api::Update> long_poll(api::telegram api, api::arrayof<api::String> allowed_updates = {},
                                   bool drop_pending_updates = false);

}  // namespace tgbm
