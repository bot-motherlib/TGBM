#pragma once

#include "tgbm/net/HttpClient.h"
#include "tgbm/net/HttpParser.h"
#include "tgbm/net/ConnectionPool.h"
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

/**
 * @brief This class makes http requests via boost::asio.
 *
 * @ingroup net
 */
// TODO? shared from this?...
class TGBM_API BoostHttpOnlySslClient : public HttpClient {
 public:
  using connection_t = std::shared_ptr<asio_connection_t>;

  BoostHttpOnlySslClient(boost::asio::io_context &ctx KELCORO_LIFETIMEBOUND, std::string host,
                         size_t connections_max_count = 1000);
  ~BoostHttpOnlySslClient() override;

  /**
   * @brief Sends a request to the url.
   *
   * If there's no args specified, a GET request will be sent, otherwise a POST request will be sent.
   * If at least 1 arg is marked as file, the content type of a request will be multipart/form-data, otherwise
   * it will be application/x-www-form-urlencoded.
   */
  dd::task<std::string> makeRequest(http_request) override;

  static dd::task<connection_t> create_connection(boost::asio::io_context &, std::string host);
  [[nodiscard]] boost::asio::io_context &get_io_context() noexcept {
    return io_ctx;
  }

 private:
  struct log_events_handler_t {
    static void dropped(const connection_t &c) {
      LOG("[socket] {} dropped", (void *)&c);
    }
    static void reused(const connection_t &c) {
      LOG("[socket] {} reused", (void *)&c);
    }
    static void created(const connection_t &c) {
      LOG("[socket] {} created", (void *)&c);
    }
    static void deleted(const connection_t &c) {
      LOG("[socket] {} deleted", (void *)&c);
    }
  };

  boost::asio::io_context &io_ctx;
  const HttpParser _httpParser;
  pool_t<connection_t, log_events_handler_t> connections;
  dd::this_thread_executor_t exe;  // TODO change
};

}  // namespace tgbm
