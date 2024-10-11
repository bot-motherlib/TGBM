#pragma once
// TODO wrap asio/detail/config.hpp (using include_next)
#include "boost/smart_ptr/intrusive_ptr.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
// windows is really bad

#undef NO_ERROR
#undef Yield
#undef min
#undef max

#include "tgbm/tools/macro.hpp"
#include "tgbm/net/ssl_context.hpp"

#include <kelcoro/task.hpp>

namespace tgbm {

namespace asio = boost::asio;

struct tcp_connection_options {
  uint32_t send_buffer_size = 1024 * 1024 * 4;     // 4 MB
  uint32_t receive_buffer_size = 1024 * 1024 * 4;  // 4 MB
  // adds delay (waiting for new requests to merge them)
  bool merge_small_requests = false;
  bool is_primal_connection = true;
  /*
    use only for testing,

    if you are receiving error with ssl hanfshake,
    add verify path for your certificate, specially on windows, where default path may be unreachable
    you can download default cerifiers here: (https://curl.se/docs/caextract.html)
    then set TGBM_SSL_ADDITIONAL_CERTIFICATE_PATH and recompile program
  */
  bool disable_ssl_certificate_verify = false;
};

struct tcp_connection;

using tcp_connection_ptr = boost::intrusive_ptr<tcp_connection>;

// one tcp connection must be used in one thread only
struct tcp_connection {
 private:
  size_t refcount = 0;

 public:
  // required all time while socket alive, may be shared between connections
  ssl_context_ptr sslctx;
  asio::ssl::stream<asio::ip::tcp::socket> socket;

  // precondition: _ssl_ctx != nullptr
  explicit tcp_connection(asio::io_context& ctx KELCORO_LIFETIMEBOUND, ssl_context_ptr _ssl_ctx)
      : sslctx(std::move(_ssl_ctx)), socket(ctx, sslctx->ctx) {
  }
  tcp_connection(tcp_connection&&) = default;
  tcp_connection& operator=(tcp_connection&&) = default;

  ~tcp_connection() {
    shutdown();
  }
  // terminates on unknown errors
  void shutdown() noexcept;

  friend void intrusive_ptr_add_ref(tcp_connection* p) noexcept {
    ++p->refcount;
  }
  friend void intrusive_ptr_release(tcp_connection* p) noexcept {
    --p->refcount;
    if (p == 0)
      delete p;
  }

  // creates client connection
  static dd::task<tcp_connection_ptr> create(asio::io_context& KELCORO_LIFETIMEBOUND, std::string host,
                                             ssl_context_ptr ctx, tcp_connection_options = {});
};

}  // namespace tgbm
