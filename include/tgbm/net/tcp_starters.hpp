#pragma once

#include <kelcoro/task.hpp>

#include <hidi/utils/deadline.hpp>
#include <hidi/asio/aio_context.hpp>

#include "tgbm/utils/fn_ref.hpp"
#include "tgbm/utils/deadline.hpp"

namespace tgbm {

// invoked after TCP handshake, before TLS handshake
using starter_t = move_only_fn<dd::task<void>(boost::asio::ip::tcp::socket&, deadline_t) const>;

dd::task<void> start_socks5(boost::asio::ip::tcp::socket& socket, std::string target_host,
                            uint16_t target_port, deadline_t);

[[nodiscard]] starter_t socks5_starter(std::string target_host = "api.telegram.org",
                                       uint16_t target_port = 443);

}  // namespace tgbm
