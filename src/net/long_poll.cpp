#include "tgbm/net/long_poll.hpp"

#include <cstdint>

#include "tgbm/net/errors.hpp"
#include "tgbm/api/allowed_updates.hpp"

namespace tgbm {

static dd::channel<api::Update> long_poll_without_preconfirm(api::telegram tg,
                                                             api::get_updates_request request,
                                                             duration_t network_timeout) {
  for (;;) {
    api::arrayof<api::Update> updates = co_await tg.getUpdates(request, deadline_after(network_timeout));
    for (api::Update& item : updates) {
      if (item.update_id >= request.offset)
        request.offset = item.update_id + 1;
      co_yield std::move(item);
    }
  }
}

dd::channel<api::Update> long_poll(api::telegram tg, std::int32_t limit, std::chrono::seconds timeout,
                                   api::arrayof<api::String> allowed_updates, bool drop_pending_updates) {
  // validate

  if (limit > 100 || limit < 1)
    throw bad_request("getUpdates 'limit': only values 1 to 100 accepted");
  if (timeout.count() < 0)
    throw bad_request("getUpdates timeout must be >= 0");
  for (std::string_view str : allowed_updates)
    if (!api::allowed_updates::is_valid_update_category(str))
      throw bad_request(fmt::format("\"{}\" is not valid update name", str));

  // fill first request

  api::get_updates_request req;
  using namespace std::chrono_literals;
  duration_t network_timeout = duration_t::max() - timeout <= 5s ? duration_t::max() : timeout + 5s;
  assert(network_timeout >= timeout);
  if (limit != 100)
    req.limit = limit;
  if (timeout.count() != 0)
    req.timeout = timeout.count();
  if (!allowed_updates.empty())
    req.allowed_updates = std::move(allowed_updates);

  // send first request with 'allowed_updates' (telegram will remember it)

  auto upts = co_await tg.getUpdates(req, deadline_after(network_timeout));
  for (api::Update& u : upts) {
    if (!req.offset || req.offset->value <= u.update_id)
      req.offset = u.update_id + 1;
  }
  if (drop_pending_updates && !upts.empty())
    (void)co_await tg.getUpdates({.offset = req.offset, .limit = 1, .timeout = 0});
  else {
    for (api::Update& u : upts)
      co_yield std::move(u);
  }
  // setted only on first sending, in future previous value used by telegram
  req.allowed_updates = std::nullopt;

  co_yield dd::elements_of(long_poll_without_preconfirm(std::move(tg), std::move(req), network_timeout));
}

}  // namespace tgbm
