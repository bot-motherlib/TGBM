#pragma once
// TODO wrap asio/detail/config.hpp (using include_next)
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
// windows is really bad

#undef NO_ERROR
#undef Yield
#undef min
#undef max

#include "tgbm/net/errors.hpp"
#include "tgbm/utils/macro.hpp"
#include "tgbm/net/ssl_context.hpp"
#include "tgbm/logger.hpp"

#include <kelcoro/task.hpp>

namespace tgbm {

namespace asio = boost::asio;

struct tcp_connection_options {
  uint32_t send_buffer_size = 1024 * 1024 * 4;     // 4 MB
  uint32_t receive_buffer_size = 1024 * 1024 * 4;  // 4 MB
  std::vector<std::filesystem::path> additional_ssl_certificates;
  // adds delay (waiting for new requests to merge them)
  bool merge_small_requests = false;
  bool is_primal_connection = true;
  /*
    default true, because in most cases (windows...) it will produce errors until you set
    'additional_ssl_certificates'

    if you are receiving error with ssl hanfshake,
    add verify path for your certificate, specially on windows, where default path may be unreachable
    you can download default cerifiers here: (https://curl.se/docs/caextract.html)
  */
  bool disable_ssl_certificate_verify = true;

  template <typename E>
  void apply(asio::basic_socket<asio::ip::tcp, E>& tcp_sock) {
    using tcp = asio::ip::tcp;

    tcp_sock.set_option(tcp::no_delay(!merge_small_requests));
    {
      asio::socket_base::send_buffer_size send_sz_option(send_buffer_size);
      tcp_sock.set_option(send_sz_option);
      tcp_sock.get_option(send_sz_option);
      if (send_sz_option.value() != send_buffer_size) {
        TGBM_LOG_WARN("tcp sendbuf size option not fully applied, requested: {}, actual: {}",
                      send_buffer_size, send_sz_option.value());
      }
    }
    {
      asio::socket_base::receive_buffer_size rsv_sz_option(receive_buffer_size);
      tcp_sock.set_option(rsv_sz_option);
      tcp_sock.get_option(rsv_sz_option);
      if (rsv_sz_option.value() != send_buffer_size) {
        TGBM_LOG_WARN("tcp receive buf size option not fully applied, requested: {}, actual: {}",
                      send_buffer_size, rsv_sz_option.value());
      }
    }
  }
};

struct tcp_connection;

using tcp_connection_ptr = boost::intrusive_ptr<tcp_connection>;

// one tcp connection must be used in one thread only
struct tcp_connection {
 private:
  size_t refcount = 0;

  // private, must not be created on stack
  ~tcp_connection() {
    shutdown();
  }
  friend struct http2_connection;

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
