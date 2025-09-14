#pragma once

#include <string_view>

#include <kelcoro/task.hpp>

#include <anyany/anyany.hpp>

#include <tgbm/net/errors.hpp>
#include <tgbm/net/http_base.hpp>
#include <tgbm/utils/fn_ref.hpp>
#include <tgbm/utils/memory.hpp>
#include <tgbm/utils/deadline.hpp>

namespace tgbm {

struct http_client {
  virtual ~http_client() = default;

  virtual std::string_view get_host() const noexcept = 0;

  // rethrows exceptions from 'on_header' and 'on_data_part' to caller
  // if 'on_header' is nullptr, all headers ignored (status parsed if 'on_data_part' != nullptr)
  // if 'on_data_part' is nullptr, then server answer ignored
  // if both nullptr, then request only sended and then returns immediately
  // returns < 0 if error (reqerr_e),
  // 0 if request done, but both handlers nullptr and status not parsed
  // > 0 if 3-digit server response code
  virtual dd::task<int> send_request(on_header_fn_ptr, on_data_part_fn_ptr, http_request, deadline_t) = 0;

  virtual dd::task<void> sleep(duration_t, io_error_code&) = 0;

  // run until all work done
  virtual void run() = 0;
  // run until some work done, returns false if no one executed
  virtual bool run_one(duration_t timeout) = 0;
  virtual void stop() = 0;
};

}  // namespace tgbm
