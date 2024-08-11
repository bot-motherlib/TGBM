#pragma once

#include "tgbm/net/HttpClient.h"
#include "tgbm/net/Url.h"
#include "tgbm/net/HttpReqArg.h"
#include "tgbm/net/HttpParser.h"
#include "tgbm/net/ConnectionPool.h"

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
  using connection_t = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

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
  // TOOD ссылки опасные, могут протухнутьы
  dd::task<std::string> makeRequest(const Url& url, const std::vector<HttpReqArg>& args) override;

  static dd::task<connection_t> create_connection(boost::asio::io_context&, std::string host);
  [[nodiscard]] boost::asio::io_context& getIoContext() noexcept {
    return io_ctx;
  }

 private:
  mutable boost::asio::io_context io_ctx;
  const HttpParser _httpParser;
  pool_t<connection_t> connections;
  dd::this_thread_executor_t exe;  // TODO change
};

}  // namespace tgbm
