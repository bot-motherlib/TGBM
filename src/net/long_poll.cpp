#include "tgbm/net/long_poll.hpp"

#include <cstdint>

#include "tgbm/logger.hpp"
#include "tgbm/net/errors.hpp"

namespace tgbm {

static dd::channel<api::Update> long_poll_with_preconfirm(api::telegram tg, api::get_updates_request request,
                                                          duration_t network_timeout) {
  for (;;) {
    api::arrayof<api::Update> updates = co_await tg.getUpdates(request, network_timeout);
    if (updates.empty())
      continue;

    for (api::Update& u : updates) {
      if (u.update_id >= request.offset)
        request.offset = u.update_id + 1;
    }
    // mark updates as handled before handling them
    // ignores update, so its unhanled == will be returned next time
    (void)co_await tg.getUpdates({
        .offset = request.offset,
        .limit = 1,
        .timeout = 0,
        .allowed_updates = request.allowed_updates,
    });
    TGBM_LOG_INFO("[updates] getted {} updates", updates.size());
    for (api::Update& item : updates)
      co_yield std::move(item);
  }
}

static dd::channel<api::Update> long_poll_without_preconfirm(api::telegram tg,
                                                             api::get_updates_request request,
                                                             duration_t network_timeout) {
  for (;;) {
    api::arrayof<api::Update> updates = co_await tg.getUpdates(request, network_timeout);
    TGBM_LOG_INFO("[updates] getted {} updates", updates.size());
    for (api::Update& item : updates) {
      if (item.update_id >= request.offset)
        request.offset = item.update_id + 1;
      co_yield std::move(item);
    }
  }
}

dd::channel<api::Update> long_poll(api::telegram tg, std::int32_t limit, std::chrono::seconds timeout,
                                   api::arrayof<api::String> allow_updates, bool confirm_before_handle) {
  if (limit > 100 || limit < 1)
    throw bad_request("getUpdates 'limit': only values 1 to 100 accepted");
  if (timeout.count() < 0)
    throw bad_request("getUpdates timeout must be >= 0");
  api::get_updates_request req;
  using namespace std::chrono_literals;
  duration_t network_timeout = duration_t::max() - timeout <= 5s ? duration_t::max() : timeout + 5s;
  assert(network_timeout >= timeout);
  if (limit != 100)
    req.limit = limit;
  if (timeout.count() != 0)
    req.timeout = timeout.count();
  if (!allow_updates.empty())
    req.allowed_updates = std::move(allow_updates);
  if (confirm_before_handle)
    return long_poll_with_preconfirm(std::move(tg), std::move(req), network_timeout);
  return long_poll_without_preconfirm(std::move(tg), std::move(req), network_timeout);
}

}  // namespace tgbm
