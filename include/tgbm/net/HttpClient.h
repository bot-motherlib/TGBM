#ifndef TGBOT_HTTPCLIENT_H
#define TGBOT_HTTPCLIENT_H

#include "tgbm/net/Url.h"
#include "tgbm/net/HttpReqArg.h"

#include <string>
#include <vector>
#include <cstdint>

#include <kelcoro/task.hpp>

namespace tgbm {

/**
 * @brief This class makes http requests.
 *
 * @ingroup net
 */
class TGBOT_API HttpClient {
 public:
  virtual ~HttpClient() = default;

  // TODO add prepare function?

  /**
   * @brief Sends a request to the url.
   *
   * If there's no args specified, a GET request will be sent, otherwise a POST request will be sent.
   * If at least 1 arg is marked as file, the content type of a request will be multipart/form-data, otherwise
   * it will be application/x-www-form-urlencoded.
   */
  // TODO вот здесь главный асинх должен произойти
  // TODO? сразу сюда принимать не аргументы, а сгенеренный body?. Крч надо сразу json сделать и положить его
  // в application/json, вместо сначала создания вектора
  virtual dd::task<std::string> makeRequest(const Url& url, const std::vector<HttpReqArg>& args) const = 0;
  // TODO передавать таймаут в makeRequest, количество ретраев вероятно тоже. И std::chrono::seconds вместо
  // int
  std::int32_t _timeout = 25;

  /**
   * @brief Get the maximum number of makeRequest() retries before giving up and throwing an exception.
   */
  virtual int getRequestMaxRetries() const {
    return requestMaxRetries;
  }

  /**
   * @brief Get the makeRequest() backoff duration between retries, in seconds.
   */
  virtual int getRequestBackoff() const {
    return requestBackoff;
  }

 private:
  int requestMaxRetries = 3;
  int requestBackoff = 1;
};

}  // namespace tgbm

#endif  // TGBOT_HTTPCLIENT_H
