#pragma once

#include <boost/asio/ip/tcp.hpp>

#include <kelcoro/task.hpp>

#include <tgbm/net/any_connection.hpp>

namespace tgbm {

// any_acceptor interface

ANYANY_METHOD(accept, dd::task<any_connection>(io_error_code& ec));

/*
required intereface:
  // returns empty connection with setted 'ec' on errors
  any_connection accept(io_error_code& ec);
*/
using any_acceptor = aa::basic_any_with<aa::default_allocator, 0, accept_m>;

template <typename T, typename... Args>
any_acceptor make_any_acceptor(Args&&... args) {
  return aa::make_any<any_acceptor, T>(std::forward<Args>(args)...);
}

}  // namespace tgbm
