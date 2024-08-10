#ifndef TGBOT_BOOSTHTTPCLIENT_H
#define TGBOT_BOOSTHTTPCLIENT_H

#include "tgbm/net/HttpClient.h"
#include "tgbm/net/Url.h"
#include "tgbm/net/HttpReqArg.h"
#include "tgbm/net/HttpParser.h"

#include <boost/asio.hpp>

#include <string>
#include <vector>

namespace tgbm {

/**
 * @brief This class makes http requests via boost::asio.
 *
 * @ingroup net
 */
class TGBOT_API BoostHttpOnlySslClient : public HttpClient {
 public:
  BoostHttpOnlySslClient();
  ~BoostHttpOnlySslClient() override;

  /**
   * @brief Sends a request to the url.
   *
   * If there's no args specified, a GET request will be sent, otherwise a POST request will be sent.
   * If at least 1 arg is marked as file, the content type of a request will be multipart/form-data, otherwise
   * it will be application/x-www-form-urlencoded.
   */
  // TODO понять правильно ли передаются & в ключе и значении x-www-form-urlencoded (должно быть %26 и тд)
  dd::task<std::string> makeRequest(const Url& url, const std::vector<HttpReqArg>& args) const override;

  [[nodiscard]] boost::asio::io_service& getIoService() noexcept {
    return _ioService;
  }

 private:
  mutable boost::asio::io_service _ioService;
  const HttpParser _httpParser;
  // TODO здесь кешировать сокет (или стрим и тд)
};

}  // namespace tgbm

#endif  // TGBOT_BOOSTHTTPCLIENT_H
