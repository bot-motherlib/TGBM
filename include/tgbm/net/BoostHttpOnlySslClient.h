#pragma once

#include "tgbm/net/HttpClient.h"
#include "tgbm/net/Url.h"
#include "tgbm/net/HttpReqArg.h"
#include "tgbm/net/HttpParser.h"
#include "tgbm/net/ConnectionPool.h"
#include "tgbm/logger.h"

#include <boost/asio/ssl.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>
#include <vector>

namespace tgbm {

/**
 * @brief This class makes http requests via boost::asio.
 *
 * @ingroup net
 */
// TODO? shared from this?...
class TGBM_API BoostHttpOnlySslClient : public HttpClient {
  struct connection_t {
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket;
    bool used = false;
  };

 public:
  BoostHttpOnlySslClient(std::string host, size_t connections_max_count = 1000);
  ~BoostHttpOnlySslClient() override;

  /**
   * @brief Sends a request to the url.
   *
   * If there's no args specified, a GET request will be sent, otherwise a POST request will be sent.
   * If at least 1 arg is marked as file, the content type of a request will be multipart/form-data, otherwise
   * it will be application/x-www-form-urlencoded.
   */
  // TODO понять правильно ли передаются & в ключе и значении x-www-form-urlencoded (должно быть %26 и тд)
  // TODO разделить get (без аргментов)/post и отдельно видимо файл, т.е. будет одно application/json
  // и второе multipart/form-data
  dd::task<std::string> makeRequest(Url url, std::vector<HttpReqArg> args) override;

  static dd::task<connection_t> create_connection(boost::asio::io_context &, std::string host);
  [[nodiscard]] boost::asio::io_context &getIoContext() noexcept {
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

  mutable boost::asio::io_context io_ctx;
  const HttpParser _httpParser;
  pool_t<connection_t, log_events_handler_t> connections;
  dd::this_thread_executor_t exe;  // TODO change
};

}  // namespace tgbm
