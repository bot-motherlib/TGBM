#pragma once

#ifdef _WIN32
  #undef Yield
#endif
#include <kelcoro/channel.hpp>

#include <tgbm/api/types/all.hpp>
#include <tgbm/api/telegram.hpp>
#include <tgbm/api/arrayof.hpp>
#include <tgbm/api/allowed_updates.hpp>

namespace tgbm {

struct long_poll_options {
  api::arrayof<api::String> allowed_updates = {};
  bool drop_pending_updates = false;
  // timeout for each getUpdates request (not network timeout, but API timeout)
  std::chrono::seconds timeout = std::chrono::seconds(10);
};

// Note: getUpdates does not work until webHooks are deleted
// so api.deleteWebhook() is called before polling
dd::channel<api::Update> long_poll(api::telegram api, long_poll_options = {});

}  // namespace tgbm
