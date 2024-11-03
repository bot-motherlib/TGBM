#pragma once

#include <string>
#include <string_view>

#include <kelcoro/task.hpp>

#include <anyany/anyany.hpp>

#include "tgbm/net/http_base.hpp"
#include "tgbm/net/HttpParser.h"
#include "tgbm/tools/fn_ref.hpp"
#include "tgbm/tools/memory.hpp"
#include "tgbm/tools/deadline.hpp"

namespace tgbm {

using on_header_fn_ptr = fn_ptr<void(std::string_view name, std::string_view value)>;

using on_data_part_fn_ptr = fn_ptr<void(std::span<const byte_t> bytes, bool last_part)>;
using on_data_part_fn_ref = fn_ref<void(std::span<const byte_t> bytes, bool last_part)>;

struct http_client {
 protected:
  std::string host;

 public:
  std::string_view get_host() const noexcept {
    return host;
  }
  // 'host' used for
  //   * connecting when required
  //   * SSL host name verification
  //   * as default host if request host not setted
  //  but 'host' header in requests may differ
  // note: same as :authority for HTTP2
  http_client(std::string_view host);

  virtual ~http_client() = default;

  // rethrows exceptions from 'on_header' and 'on_data_part' to caller
  // if 'on_header' is nullptr, all headers ignored (status parsed if 'on_data_part' != nullptr)
  // if 'on_data_part' is nullptr, then server answer ignored
  // if both nullptr, then request only sended and then returns immediately
  // returns < 0 if error (reqerr_e),
  // 0 if request done, but both handlers nullptr and status not parsed
  // > 0 if 3-digit server response code
  virtual dd::task<int> send_request(on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part,
                                     http_request request, duration_t timeout) = 0;

  // throws on errors
  dd::task<http_response> send_request(http_request request, duration_t timeout);

  // run until all work done
  virtual void run() = 0;
  // run until some work done, returns false if no one executed
  virtual bool run_one(duration_t timeout) = 0;
  virtual void stop() = 0;
};

}  // namespace tgbm
