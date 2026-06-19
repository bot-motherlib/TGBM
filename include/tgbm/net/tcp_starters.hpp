#pragma once

#include <kelcoro/task.hpp>

#include <http2/utils/deadline.hpp>
#include <http2/asio/aio_context.hpp>

#include "tgbm/utils/fn_ref.hpp"

namespace tgbm {

// invoked after TCP handshake, before TLS handshake
using starter_t = move_only_fn<dd::task<void>(boost::asio::ip::tcp::socket&, http2::deadline_t) const>;

dd::task<void> start_socks5(boost::asio::ip::tcp::socket& socket, std::string target_host,
                            uint16_t target_port, http2::deadline_t);

[[nodiscard]] starter_t socks5_starter(std::string target_host = "api.telegram.org",
                                       uint16_t target_port = 443);

}  // namespace tgbm
