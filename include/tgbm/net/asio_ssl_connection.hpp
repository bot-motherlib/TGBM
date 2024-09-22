#pragma once
// TODO wrap asio/detail/config.hpp (using include_next)
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
// windows is really bad

#undef NO_ERROR
#undef Yield
#undef min
#undef max

#include "tgbm/tools/macro.hpp"

#include <kelcoro/task.hpp>

namespace tgbm {

namespace asio = boost::asio;

struct asio_ssl_connection : std::enable_shared_from_this<asio_ssl_connection> {
  asio::ssl::context sslctx;  // required all time while socket alive
  asio::ssl::stream<asio::ip::tcp::socket> socket;

  explicit asio_ssl_connection(asio::io_context& ctx KELCORO_LIFETIMEBOUND, asio::ssl::context _ssl_ctx)
      : sslctx(std::move(_ssl_ctx)), socket(ctx, sslctx) {
  }
  asio_ssl_connection(asio_ssl_connection&&) = default;
  asio_ssl_connection& operator=(asio_ssl_connection&&) = default;

  ~asio_ssl_connection() {
    shutdown();
  }
  // terminates on unknown errors
  void shutdown() noexcept;

  static dd::task<std::shared_ptr<asio_ssl_connection>> create(asio::io_context& KELCORO_LIFETIMEBOUND,
                                                               std::string host, asio::ssl::context ctx);
};

}  // namespace tgbm
