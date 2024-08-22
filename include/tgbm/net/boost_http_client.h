#pragma once

#include "tgbm/net/HttpClient.h"
#include "tgbm/net/connectiion_pool.h"
#include "tgbm/logger.h"

// TODO wrap asio/detail/config.hpp (using include_next)
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>

namespace tgbm {

struct asio_connection_t : std::enable_shared_from_this<asio_connection_t> {
  boost::asio::ssl::context sslctx;  // required all time while socket alive
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket;

  explicit asio_connection_t(boost::asio::io_context &ctx KELCORO_LIFETIMEBOUND,
                             boost::asio::ssl::context _ssl_ctx)
      : sslctx(std::move(_ssl_ctx)), socket(ctx, sslctx) {
  }
};
// TODO чекнуть работу таймаутов на очень маленьком таймауте каком то
// reuses memory of http_request, so lvalue ref
dd::task<http_response> send_request(std::shared_ptr<asio_connection_t>, http_request &KELCORO_LIFETIMEBOUND);
dd::task<std::shared_ptr<asio_connection_t>> create_connection(boost::asio::io_context &KELCORO_LIFETIMEBOUND,
                                                               std::string host);

struct log_events_handler_t {
  static void dropped(const std::shared_ptr<asio_connection_t> &c) {
    LOG("[socket] {} dropped", (void *)&c->socket);
  }
  static void reused(const std::shared_ptr<asio_connection_t> &c) {
    LOG("[socket] {} reused", (void *)&c->socket);
  }
  static void created(const std::shared_ptr<asio_connection_t> &c) {
    LOG("[socket] {} created", (void *)&c->socket);
  }
  static void deleted(const std::shared_ptr<asio_connection_t> &c) {
    LOG("[socket] {} deleted", (void *)&c->socket);
  }
};

struct boost_singlethread_client : HttpClient {
 private:
  // invariant: .run() invoked on 0 or 1 threads
  boost::asio::io_context io_ctx;
  pool_t<std::shared_ptr<asio_connection_t>, log_events_handler_t> connections;
  KELCORO_NO_UNIQUE_ADDRESS dd::this_thread_executor_t exe;

 public:
  explicit boost_singlethread_client(std::string host, size_t connections_max_count = 1000)
      : io_ctx(1),
        connections(
            connections_max_count,
            [io = &io_ctx, h = std::move(host)]() { return create_connection(*io, h); }, exe) {
  }

  dd::task<http_response> send_request(http_request, duration_t timeout) override;

  void run() override;

  bool run_one(duration_t timeout) override;

  void stop() override;
};

}  // namespace tgbm
