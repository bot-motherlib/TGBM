#include "tgbm/net/long_poll.hpp"

#include "tgbm/net/errors.hpp"
#include "tgbm/api/allowed_updates.hpp"

namespace tgbm {

static dd::channel<api::Update> long_poll_loop(api::telegram api, api::get_updates_request request,
                                               duration_t timeout) {
  for (;;) {
    api::arrayof<api::Update> updates = co_await api.getUpdates(request, deadline_after(timeout));
    for (api::Update& item : updates) {
      if (item.update_id >= request.offset)
        request.offset = item.update_id + 1;
      co_yield std::move(item);
    }
  }
}
dd::channel<api::Update> long_poll(api::telegram api, long_poll_options options) {
  // validate

  for (std::string_view str : options.allowed_updates)
    if (!api::allowed_updates::is_valid_update_category(str))
      throw bad_request(fmt::format("\"{}\" is not valid update name", str));

  // fill first request

  api::get_updates_request req;
  using namespace std::chrono;
  // telegram automatically answeres 0 updates after 50 seconds
  seconds timeout(50);
  req.timeout = 45;
  // req.limit == 100 by default
  if (!options.allowed_updates.empty())
    req.allowed_updates = options.allowed_updates;

  // if webhook exist, long poll is not available

  (void)co_await api.deleteWebhook({.drop_pending_updates = options.drop_pending_updates},
                                   deadline_after(5s));

  // send first request with 'allowed_updates' (telegram will remember it)

  auto upts = co_await api.getUpdates(req, deadline_after(timeout));
  req.allowed_updates = std::nullopt;
  req.offset = 0;

  for (api::Update& u : upts) {
    if (req.offset->value <= u.update_id)
      req.offset = u.update_id + 1;
    co_yield std::move(u);
  }
  co_yield dd::elements_of(long_poll_loop(std::move(api), std::move(req), timeout));
}

}  // namespace tgbm
