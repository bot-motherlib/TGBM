#pragma once

#include <type_traits>

#include <boost/asio/ssl.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>

#include <kelcoro/task.hpp>

namespace tgbm {

using io_error_code = boost::system::error_code;

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
      if (ec == boost::asio::error::operation_aborted) {
        handle.destroy();
        return;
      }
      _ec = ec;
      std::construct_at(std::addressof(_data), std::move(data));
      handle.resume();
    };
    auto cb_user = std::move(_cb_user);
    std::destroy_at(std::addressof(_cb_user));
    cb_user(cb);
  }

  [[nodiscard]] T await_resume() noexcept(std::is_nothrow_move_constructible_v<T>) {
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
  boost::asio::ip::basic_resolver<Protocol>& resolver;
  boost::asio::ip::basic_resolver_query<Protocol> query;

  template <typename T>
  void operator()(T&& cb) {
    resolver.async_resolve(std::move(query), std::forward<T>(cb));
  }
};

template <typename Protocol>
KELCORO_CO_AWAIT_REQUIRED [[clang::always_inline]] auto async_resolve(
    boost::asio::ip::basic_resolver<Protocol>& resolver KELCORO_LIFETIMEBOUND,
    std::type_identity_t<boost::asio::ip::basic_resolver_query<Protocol>> query,
    io_error_code& ec KELCORO_LIFETIMEBOUND) {
  using resolve_results_t = typename boost::asio::ip::basic_resolver<Protocol>::results_type;
  return asio_awaiter<resolve_results_t, resolve_operation<Protocol>>(ec, resolver, std::move(query));
}

template <typename Protocol>
struct connect_operation {
  boost::asio::basic_socket<Protocol>& socket;
  typename boost::asio::ip::basic_resolver<Protocol>::results_type endpoints;

  template <typename T>
  void operator()(T&& cb) {
    boost::asio::async_connect(socket, endpoints, std::forward<T>(cb));
  }
};

template <typename Protocol>
KELCORO_CO_AWAIT_REQUIRED [[clang::always_inline]] auto async_connect(
    boost::asio::basic_socket<Protocol>& socket KELCORO_LIFETIMEBOUND,
    std::type_identity_t<typename boost::asio::ip::basic_resolver<Protocol>::results_type> endpoints,
    io_error_code& ec KELCORO_LIFETIMEBOUND) {
  return asio_awaiter<boost::asio::ip::basic_endpoint<Protocol>, connect_operation<Protocol>>(
      ec, socket, std::move(endpoints));
}

template <typename Stream>
struct ssl_handshake_operation {
  boost::asio::ssl::stream<Stream>& stream;
  boost::asio::ssl::stream_base::handshake_type type;

  template <typename T>
  void operator()(T&& cb) {
    stream.async_handshake(type, std::forward<T>(cb));
  }
};

template <typename Stream>
KELCORO_CO_AWAIT_REQUIRED [[clang::always_inline]] auto async_handshake(
    boost::asio::ssl::stream<Stream>& stream KELCORO_LIFETIMEBOUND,
    boost::asio::ssl::stream_base::handshake_type type, io_error_code& ec KELCORO_LIFETIMEBOUND) {
  return asio_awaiter<void, ssl_handshake_operation<Stream>>(ec, stream, type);
}

template <typename Stream, typename ConstBuffer>
struct write_operation {
  Stream& stream;
  const ConstBuffer& buffer;

  template <typename T>
  void operator()(T&& cb) {
    boost::asio::async_write(stream, buffer, std::forward<T>(cb));
  }
};

template <typename Stream, typename ConstBuffer>
KELCORO_CO_AWAIT_REQUIRED [[clang::always_inline]] auto async_write(
    Stream& stream KELCORO_LIFETIMEBOUND, const ConstBuffer& buffer KELCORO_LIFETIMEBOUND,
    io_error_code& ec KELCORO_LIFETIMEBOUND) {
  return asio_awaiter<size_t, write_operation<Stream, ConstBuffer>>(ec, stream, buffer);
}

template <typename Stream, typename Buffer>
struct read_operation {
  Stream& stream;
  Buffer& buffer;

  template <typename T>
  void operator()(T&& cb) {
    boost::asio::async_read(stream, buffer, std::forward<T>(cb));
  }
};

template <typename Stream, typename Buffer>
KELCORO_CO_AWAIT_REQUIRED [[clang::always_inline]] auto async_read(Stream& stream KELCORO_LIFETIMEBOUND,
                                                                   Buffer&& buffer KELCORO_LIFETIMEBOUND,
                                                                   io_error_code& ec KELCORO_LIFETIMEBOUND) {
  return asio_awaiter<size_t, read_operation<Stream, std::remove_reference_t<Buffer>>>(ec, stream, buffer);
}

template <typename Stream, typename Buffer>
struct read_some_operation {
  Stream& stream;
  Buffer& buffer;

  template <typename T>
  void operator()(T&& cb) {
    stream.async_read_some(buffer, std::forward<T>(cb));
  }
};

template <typename Stream, typename Buffer>
KELCORO_CO_AWAIT_REQUIRED [[clang::always_inline]] auto async_read_some(
    Stream& stream KELCORO_LIFETIMEBOUND, Buffer&& buffer KELCORO_LIFETIMEBOUND,
    io_error_code& ec KELCORO_LIFETIMEBOUND) {
  return asio_awaiter<size_t, read_some_operation<Stream, Buffer>>(ec, stream, buffer);
}

template <typename Stream, typename Buffer>
struct read_until_operation {
  Stream& stream;
  Buffer& buffer;
  std::string_view delim;

  template <typename T>
  void operator()(T&& cb) {
    boost::asio::async_read_until(stream, buffer, delim, std::forward<T>(cb));
  }
};

// appends readen bytes to 'buffer'
// returns position where in result 'needle' starts
// TODO noexcept task?
// TODO tests на эту функцию
dd::task<size_t> do_read_until(auto& stream, std::string& buffer, std::string_view needle,
                               io_error_code& ec KELCORO_LIFETIMEBOUND) {
  assert(!needle.empty());
  assert(!ec);
  char buf[128];
  const size_t init_sz = buffer.size();
  size_t transfered = 0;
  size_t checked_bytes = 0;
  for (;;) {
    size_t read_bytes = co_await async_read_some(stream, boost::asio::buffer(buf), ec);
    if (ec)
      co_return needle.npos;
    transfered += read_bytes;
    buffer += std::string_view(buf, read_bytes);  // TODO better
    if (transfered < needle.size())
      continue;
    size_t s = needle.size() - 1;
    auto p = buffer.find(needle, init_sz + (checked_bytes > s ? checked_bytes - s : 0));
    if (p != buffer.npos)
      co_return p;
    checked_bytes += read_bytes;
  }
  KELCORO_UNREACHABLE;
}

template <typename Stream, std::convertible_to<std::string_view> StringView>
KELCORO_CO_AWAIT_REQUIRED [[clang::always_inline]] auto async_read_until(
    Stream& stream KELCORO_LIFETIMEBOUND, std::string& buffer KELCORO_LIFETIMEBOUND, StringView&& delim,
    io_error_code& ec KELCORO_LIFETIMEBOUND) {
  static_assert(!std::is_rvalue_reference_v<StringView&&> || std::is_same_v<StringView, std::string_view>,
                "string delimiter will die before operation complete");
  return do_read_until(stream, buffer, std::string_view(delim), ec);
}

template <typename Timer>
struct sleep_operation {
  Timer& timer;
  std::chrono::nanoseconds duration;

  template <typename T>
  void operator()(T&& cb) {
    timer.async_wait(std::forward<T>(cb));
  }
};

template <typename Timer>
KELCORO_CO_AWAIT_REQUIRED [[clang::always_inline]] auto async_sleep(Timer& timer KELCORO_LIFETIMEBOUND,
                                                                    std::chrono::nanoseconds duration,
                                                                    io_error_code& ec KELCORO_LIFETIMEBOUND) {
  return asio_awaiter<void, sleep_operation<Timer>>(ec, timer, duration);
}

template <typename Stream, typename Buffer>
struct read_exactly_operation {
  Stream& stream;
  Buffer& buffer;
  size_t count = 0;

  template <typename T>
  void operator()(T&& cb) {
    boost::asio::async_read(stream, buffer, boost::asio::transfer_exactly(count), std::forward<T>(cb));
  }
};

template <typename Stream, typename Buffer>
KELCORO_CO_AWAIT_REQUIRED [[clang::always_inline]] auto async_read_exactly(
    Stream& stream KELCORO_LIFETIMEBOUND, Buffer&& buffer KELCORO_LIFETIMEBOUND, size_t count,
    io_error_code& ec KELCORO_LIFETIMEBOUND) {
  return asio_awaiter<size_t, read_exactly_operation<Stream, std::remove_reference_t<Buffer>>>(ec, stream,
                                                                                               buffer, count);
}

}  // namespace tgbm
