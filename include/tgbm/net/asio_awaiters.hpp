#pragma once

#include <span>
#include <type_traits>
#include "tgbm/tools/memory.hpp"
// TODO wrap asio/detail/config.hpp (using include_next)
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
// windows is really bad

#undef NO_ERROR
#undef Yield
#undef min
#undef max

#include <kelcoro/task.hpp>

/*
This file contains wrappers around asio callbacks to replace asio::use_awaitable (its completely unusable)
allows to use my coroutines instead of asio::awaitable and do not think about co_spawn etc

Also removes problems with asio overloads (there so many of them and they are completely not understandable)
and many problems with asio::buffer (it has ~50 constructor overloads)
TODO remove problems with dynamic buffers (provide functions with it)
*/

namespace tgbm {

using io_error_code = boost::system::error_code;

namespace asio = boost::asio;

template <typename T, typename CallbackUser>
struct asio_awaiter {
  static_assert(std::is_same_v<T, std::decay_t<T>>);

  union {
    CallbackUser _cb_user;
    T _data;
  };
  io_error_code& _ec;

  template <typename... Args>
  explicit asio_awaiter(io_error_code& ec KELCORO_LIFETIMEBOUND, Args&&... args)
      : _cb_user(std::forward<Args>(args)...), _ec(ec) {
  }
  ~asio_awaiter() {
    // noop, if value exist, it was destroyed in await_resume
  }
  static bool await_ready() noexcept {
    return false;
  }

  void await_suspend(std::coroutine_handle<> handle) noexcept {
    auto cb = [this, handle](const io_error_code& ec, T data) {
      _ec = ec;
      std::construct_at(std::addressof(_data), std::move(data));
      handle.resume();
    };
    auto cb_user = std::move(_cb_user);
    std::destroy_at(std::addressof(_cb_user));
    cb_user(cb);
  }

  T await_resume() noexcept(std::is_nothrow_move_constructible_v<T>) {
    T result = std::move(_data);
    std::destroy_at(std::addressof(_data));
    return result;  // nrvo
  }
};

template <typename CallbackUser>
struct asio_awaiter<void, CallbackUser> {
  // also should not use its memory after passing callback to asio!
  static_assert(std::is_trivially_destructible_v<CallbackUser>);

  CallbackUser _cb_user;
  io_error_code& _ec;

  template <typename... Args>
  explicit asio_awaiter(io_error_code& ec [[clang::lifetimebound]], Args&&... args) noexcept
      : _cb_user(std::forward<Args>(args)...), _ec(ec) {
  }

  static bool await_ready() noexcept {
    return false;
  }

  void await_suspend(std::coroutine_handle<> handle) noexcept {
    auto cb = [this, handle](const io_error_code& ec) {
      _ec = ec;
      handle.resume();
    };
    // assume does not use its memory after passing callback to asio
    _cb_user(cb);
  }
  static void await_resume() noexcept {
  }
};

template <typename Protocol>
struct resolve_operation {
  asio::ip::basic_resolver<Protocol>& resolver;
  asio::ip::basic_resolver_query<Protocol> query;

  template <typename T>
  void operator()(T&& cb) {
    resolver.async_resolve(std::move(query), std::forward<T>(cb));
  }
};

template <typename Protocol>
struct connect_operation {
  asio::basic_socket<Protocol>& socket;
  typename asio::ip::basic_resolver<Protocol>::results_type endpoints;

  template <typename T>
  void operator()(T&& cb) {
    asio::async_connect(socket, endpoints, std::forward<T>(cb));
  }
};

template <typename Stream>
struct ssl_handshake_operation {
  asio::ssl::stream<Stream>& stream;
  asio::ssl::stream_base::handshake_type type;

  template <typename T>
  void operator()(T&& cb) {
    stream.async_handshake(type, std::forward<T>(cb));
  }
};

template <typename Stream>
struct write_operation {
  Stream& stream;
  std::span<const byte_t> buffer;

  template <typename T>
  void operator()(T&& cb) {
    asio::async_write(stream, asio::buffer(buffer.data(), buffer.size()), std::forward<T>(cb));
  }
};

template <typename Stream, size_t BufCount>
struct write_many_operation {
  Stream& stream;
  std::array<asio::const_buffer, BufCount> bufs;

  template <typename... Bytes>
  write_many_operation(Stream& s, std::span<const Bytes>... spans)
      : stream(s), bufs{asio::buffer(spans.data(), spans.size_bytes())...} {
    static_assert((std::is_same_v<Bytes, byte_t> && ...));
  }

  template <typename T>
  void operator()(T&& cb) {
    asio::async_write(stream, bufs, std::forward<T>(cb));
  }
};

template <typename Stream>
struct read_operation {
  Stream& stream;
  std::span<byte_t> buffer;

  template <typename T>
  void operator()(T&& cb) {
    asio::async_read(stream, asio::buffer(buffer.data(), buffer.size()), std::forward<T>(cb));
  }
};

template <typename Stream>
struct read_some_operation {
  Stream& stream;
  std::span<byte_t> buffer;

  template <typename T>
  void operator()(T&& cb) {
    stream.async_read_some(asio::buffer(buffer.data(), buffer.size()), std::forward<T>(cb));
  }
};

template <typename Stream, typename Buffer>
struct read_until_operation {
  Stream& stream;
  std::span<byte_t> buffer;
  std::string_view delim;

  template <typename T>
  void operator()(T&& cb) {
    asio::async_read_until(stream, asio::buffer(buffer.data(), buffer.size()), delim, std::forward<T>(cb));
  }
};

template <typename Timer>
struct sleep_operation {
  Timer& timer;
  std::chrono::nanoseconds duration;

  template <typename T>
  void operator()(T&& cb) {
    timer.expires_after(duration);
    timer.async_wait(std::forward<T>(cb));
  }
};

template <typename Stream>
struct shutdown_operation {
  Stream& stream;

  template <typename T>
  void operator()(T&& cb) {
    stream.async_shutdown(std::forward<T>(cb));
  }
};

template <typename Protocol>
struct accept_operation {
  asio::basic_socket_acceptor<Protocol>& acceptor;

  template <typename T>
  void operator()(T&& cb) const {
    acceptor.async_accept(std::forward<T>(cb));
  }
};

// boost asio grabs all possible good names for functions and creates 3042350 overloads
// with all possible combinations of template arguments, tags etc, so this ignores adl
struct net_t {
  template <typename Protocol>
  KELCORO_CO_AWAIT_REQUIRED static auto accept(asio::basic_socket_acceptor<Protocol>& acceptor
                                                   KELCORO_LIFETIMEBOUND,
                                               io_error_code& ec KELCORO_LIFETIMEBOUND) {
    return asio_awaiter<asio::basic_stream_socket<Protocol>, accept_operation<Protocol>>(ec, acceptor);
  }

  // returns resolve results
  template <typename Protocol>
  KELCORO_CO_AWAIT_REQUIRED static auto resolve(
      asio::ip::basic_resolver<Protocol>& resolver KELCORO_LIFETIMEBOUND,
      std::type_identity_t<asio::ip::basic_resolver_query<Protocol>> query,
      io_error_code& ec KELCORO_LIFETIMEBOUND) {
    using resolve_results_t = typename asio::ip::basic_resolver<Protocol>::results_type;
    return asio_awaiter<resolve_results_t, resolve_operation<Protocol>>(ec, resolver, std::move(query));
  }

  // returns endpoint (to who connected)
  template <typename Protocol>
  KELCORO_CO_AWAIT_REQUIRED static auto connect(
      asio::basic_socket<Protocol>& socket KELCORO_LIFETIMEBOUND,
      std::type_identity_t<typename asio::ip::basic_resolver<Protocol>::results_type> endpoints,
      io_error_code& ec KELCORO_LIFETIMEBOUND) {
    return asio_awaiter<asio::ip::basic_endpoint<Protocol>, connect_operation<Protocol>>(
        ec, socket, std::move(endpoints));
  }

  template <typename Stream>
  KELCORO_CO_AWAIT_REQUIRED static auto handshake(asio::ssl::stream<Stream>& stream KELCORO_LIFETIMEBOUND,
                                                  asio::ssl::stream_base::handshake_type type,
                                                  io_error_code& ec KELCORO_LIFETIMEBOUND) {
    return asio_awaiter<void, ssl_handshake_operation<Stream>>(ec, stream, type);
  }

  // returns count of bytes transmitted
  template <typename Stream>
  KELCORO_CO_AWAIT_REQUIRED static auto write(Stream& stream KELCORO_LIFETIMEBOUND,
                                              std::span<const byte_t> buffer,
                                              io_error_code& ec KELCORO_LIFETIMEBOUND) {
    return asio_awaiter<size_t, write_operation<Stream>>(ec, stream, buffer);
  }
  // writes all buffers (order: from first to last)
  // returns count of bytes transmitted
  // Note: there are no guarantee, that buffers will be sended contiguously!
  // this means, many calls to 'write_many' may produce invalid sequence, if buffers was parts of one thing
  template <typename Stream, typename... Byte, size_t... Szs>
  KELCORO_CO_AWAIT_REQUIRED static auto write_many(Stream& stream KELCORO_LIFETIMEBOUND,
                                                   io_error_code& ec KELCORO_LIFETIMEBOUND,
                                                   std::span<Byte, Szs>... buffers) {
    static_assert(sizeof...(buffers) > 0);
    return asio_awaiter<size_t, write_many_operation<Stream, sizeof...(buffers)>>(
        ec, stream, reinterpret_span_as_bytes(buffers)...);
  }

  // reads until buffer is full!
  // returns count of bytes transmitted
  template <typename Stream>
  KELCORO_CO_AWAIT_REQUIRED static auto read(Stream& stream KELCORO_LIFETIMEBOUND, std::span<byte_t> buffer,
                                             io_error_code& ec KELCORO_LIFETIMEBOUND) {
    return asio_awaiter<size_t, read_operation<Stream>>(ec, stream, buffer);
  }

  // returns count of bytes transmitted
  // TODO return span to readen bytes? And what to accept? dynbuf + temp buf?
  template <typename Stream>
  KELCORO_CO_AWAIT_REQUIRED static auto read_some(Stream& stream KELCORO_LIFETIMEBOUND,
                                                  std::span<byte_t> buffer KELCORO_LIFETIMEBOUND,
                                                  io_error_code& ec KELCORO_LIFETIMEBOUND) {
    return asio_awaiter<size_t, read_some_operation<Stream>>(ec, stream, buffer);
  }

  // appends readen bytes to 'buffer'
  // returns position where 'delim' starts or -1 on error
  // precondition: needle is not empty
  static dd::task<size_t> read_until(auto& stream KELCORO_LIFETIMEBOUND,
                                     std::string& buffer KELCORO_LIFETIMEBOUND, std::string_view needle,
                                     io_error_code& ec KELCORO_LIFETIMEBOUND) {
    assert(!needle.empty());
    assert(!ec);
    // TODO better
    byte_t buf[128];
    const size_t init_sz = buffer.size();
    size_t transfered = 0;
    size_t checked_bytes = 0;
    for (;;) {
      size_t read_bytes = co_await net_t{}.read_some(stream, buf, ec);
      if (ec)
        co_return needle.npos;
      transfered += read_bytes;
      buffer += std::string_view((char*)buf, read_bytes);  // TODO better
      if (transfered < needle.size())
        continue;
      size_t s = needle.size() - 1;
      auto p = buffer.find(needle, init_sz + (checked_bytes > s ? checked_bytes - s : 0));
      if (p != buffer.npos)
        co_return p;
      checked_bytes += read_bytes;
    }
    unreachable();
  }

  // appends readen bytes to 'buffer'
  // returns count of bytes transmitted
  static dd::task<void> read_until_condition(auto& stream KELCORO_LIFETIMEBOUND,
                                             std::vector<uint8_t>& buffer KELCORO_LIFETIMEBOUND,
                                             auto condition, io_error_code& ec KELCORO_LIFETIMEBOUND) {
    assert(!ec);
    // TODO better
    byte_t buf[128];
    for (;;) {
      size_t read_bytes = co_await net_t{}.read_some(stream, buf, ec);
      if (ec)
        co_return;
      buffer.insert(buffer.end(), buf, buf + read_bytes);  // TODO better
      if (condition(buffer))
        co_return;
    }
    unreachable();
  }

  template <typename Timer>
  KELCORO_CO_AWAIT_REQUIRED static auto sleep(Timer& timer KELCORO_LIFETIMEBOUND,
                                              std::chrono::nanoseconds duration,
                                              io_error_code& ec KELCORO_LIFETIMEBOUND) {
    return asio_awaiter<void, sleep_operation<Timer>>(ec, timer, duration);
  }

  template <typename Stream>
  KELCORO_CO_AWAIT_REQUIRED static auto shutdown(Stream& stream KELCORO_LIFETIMEBOUND,
                                                 io_error_code& ec KELCORO_LIFETIMEBOUND) {
    return asio_awaiter<void, shutdown_operation<Stream>>(ec, stream);
  }
};

// async net operations without ADL
constexpr inline net_t net = {};

}  // namespace tgbm
