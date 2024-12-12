#pragma once

#include <boost/asio/ip/tcp.hpp>

#include <kelcoro/task.hpp>

#include <tgbm/utils/memory.hpp>
#include <tgbm/net/errors.hpp>
#include <tgbm/utils/deadline.hpp>
#include <tgbm/net/any_acceptor.hpp>
#include <tgbm/net/any_connection.hpp>

#include <tgbm/utils/anyany_utils.hpp>

namespace tgbm {

// any_transport_factory interface

// postcondition: .has_value() == true
ANYANY_METHOD(create_connection, dd::task<any_connection>(std::string_view host));

// Note: in server 'run' may be called on multiple threads!
ANYANY_METHOD(run, void());

ANYANY_METHOD(stop, void());

ANYANY_METHOD(run_one, bool(duration_t timeout));

ANYANY_METHOD(sleep, dd::task<void>(duration_t d, io_error_code& ec));

/*
required intereface:

  dd::task<any_connection> create_connection(std::string_view host);
  void run();
  void stop();
  bool run_one(duration_t timeout);
  dd::task<void> sleep(duration_t d, io_error_code& ec);
*/
using any_transport_factory =
    aa::basic_any_with<aa::default_allocator, 0, create_connection_m, run_m, stop_m, run_one_m, sleep_m>;

template <typename T, typename... Args>
any_transport_factory make_any_transport_factory(Args&&... args) {
  return aa::make_any<any_transport_factory, T>(std::forward<Args>(args)...);
}

[[nodiscard]] any_transport_factory default_transport_factory();

using endpoint_t = boost::asio::ip::tcp::endpoint;

ANYANY_METHOD(make_acceptor, any_acceptor(endpoint_t ep));

// should not block, initiates work
// returns false if already started
ANYANY_METHOD(start, bool());

/*
required intereface:

  // may be called concurrently
  // should block (and handle tasks) until .stop called
  void run();
  void stop();

  // must create acceptor to produce sockets for server
  any_acceptor make_acceptor(asio::ip::tcp::endpoint);

  // should not block, initiates work
  // returns false if already started
  // may be called concurrently
  bool start();

*/
using any_server_transport_factory =
    aa::basic_any_with<aa::default_allocator, 0, run_m, stop_m, make_acceptor_m, start_m>;

template <typename T, typename... Args>
any_server_transport_factory make_any_server_transport_factory(Args&&... args) {
  return aa::make_any<any_server_transport_factory, T>(std::forward<Args>(args)...);
}

}  // namespace tgbm
