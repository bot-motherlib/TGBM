#pragma once

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS

#include "tgbm/net/TgWebhookServer.h"

#include <string>

namespace tgbm {

/**
 * @brief This class setups HTTP server for receiving Telegram Update objects from unix socket.
 *
 * @ingroup net
 */
class TgWebhookLocalServer : public TgWebhookServer<boost::asio::local::stream_protocol> {
 public:
  TgWebhookLocalServer(const std::string& unixSocketPath, const std::string& path,
                       const EventHandler& eventHandler)
      : TgWebhookServer<boost::asio::local::stream_protocol>(
            boost::asio::local::stream_protocol::endpoint(unixSocketPath), path, eventHandler) {
  }

  TgWebhookLocalServer(const std::string& unixSocketPath, const Bot& bot)
      : TgWebhookServer<boost::asio::local::stream_protocol>(
            boost::asio::local::stream_protocol::endpoint(unixSocketPath), bot) {
  }
};

}  // namespace tgbm

#endif  // BOOST_ASIO_HAS_LOCAL_SOCKETS
