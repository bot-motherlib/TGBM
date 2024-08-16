#include "tgbm/net/BoostHttpOnlySslClient.h"

#include <cstddef>
#include <vector>

#include <boost/asio.hpp>

#include "tgbm/scope_exit.h"

namespace tgbm {

BoostHttpOnlySslClient::BoostHttpOnlySslClient(std::string host, size_t connections_max_count)
    : _httpParser(),
      connections(
          connections_max_count, [io = &io_ctx, h = std::move(host)]() { return create_connection(*io, h); },
          exe) {
}

BoostHttpOnlySslClient::~BoostHttpOnlySslClient() = default;
// TODO вынести в отдельный хедер
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
// TODO co_await sleep (timer, duration)
dd::task<BoostHttpOnlySslClient::connection_t> BoostHttpOnlySslClient::create_connection(
    boost::asio::io_context& io_ctx KELCORO_LIFETIMEBOUND, std::string host) {
  namespace asio = boost::asio;
  namespace ssl = asio::ssl;
  using tcp = asio::ip::tcp;

  tcp::resolver resolver(io_ctx);
  std::shared_ptr connection = [&io_ctx] {
    ssl::context sslctx(ssl::context::tlsv12_client);
    sslctx.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 |
                       ssl::context::single_dh_use);
    sslctx.set_default_verify_paths();
    return std::make_shared<asio_connection_t>(io_ctx, std::move(sslctx));
  }();
  ssl::stream<tcp::socket>& socket = connection->socket;
  // TODO ? is needed?
  tcp::resolver::query query(std::string(host), "https");
  io_error_code ec;
  auto results = co_await async_resolve(resolver, query, ec);
  if (ec)
    throw network_exception("[http] cannot resolve host: {}: service: {}, err: {}", query.host_name(),
                            query.service_name(), ec.message());
  asio::connect(socket.lowest_layer(), results, ec);
  (void)co_await async_connect(socket.lowest_layer(), results, ec);
  if (ec)
    throw network_exception("[http] cannot connect to {}, err: {}", host, ec.message());
  socket.lowest_layer().set_option(tcp::no_delay(true));
  // TODO подобрать хорошие значения
  socket.lowest_layer().set_option(asio::socket_base::send_buffer_size(32768));
  socket.lowest_layer().set_option(asio::socket_base::receive_buffer_size(32768));
  socket.set_verify_mode(ssl::verify_none);
  socket.set_verify_callback(ssl::host_name_verification(host));

  co_await async_handshake(socket, ssl::stream_base::handshake_type::client, ec);
  if (ec)
    throw network_exception("[http] cannot ssl handshake: {}", ec.message());
  co_return connection;
}

dd::task<std::string> BoostHttpOnlySslClient::makeRequest(Url url, std::vector<HttpReqArg> args) {
  namespace asio = boost::asio;
  using tcp = asio::ip::tcp;

  // TODO? retry if throw?
  auto handle = co_await connections.borrow();
  auto& socket = handle.get()->get()->socket;
  on_scope_failure(drop_socket) {
    // avoid drop connection on timeout (handle.destroy())
    if (std::uncaught_exceptions())
      handle.drop();
  };
  io_error_code ec;
  std::string requestText = _httpParser.generateRequest(url, args, /*isKeepAlive=*/true);
  size_t transfered = co_await async_write(socket, asio::buffer(requestText), ec);
  if (ec)
    throw http_exception("[http] cannot write to {} socket, err: {}", (void*)&socket, ec.message());
  assert(transfered == requestText.size());
  // TODO timeout на весь сокет сразу одну штуку(точнее на одну операцию(корутину)...)

  std::string data;
  // TODO нужно возвращать вектор байт вместо строки 1. это выгоднее (sso не нужно)
  // 2. там могут быть \0, т.к. это в том числе загрузка файлов

  // TODO read http status string first, return http status code too
  // HTTP<version><whitespaces><code>
  static constexpr std::string_view headers_end_marker = "\r\n\r\n";
  // read http headers
  std::string::size_type headers_end_pos = co_await async_read_until(socket, data, headers_end_marker, ec);
  // TODO получается тут 3 read_until - status string \r\n, Content-Length: (хотя это ... Хм), headers end
  // \r\n\r\n
  if (ec)
    throw http_exception("[http] error while reading http headers: {}", ec.message());
  static constexpr std::string_view content_length_header = "Content-Length:";
  auto content_len_pos = data.find(content_length_header);
  if (content_len_pos == data.npos)
    throw http_exception("[http] incorrect server answer");
  content_len_pos += content_length_header.size();
  // skip whitespaces (exactly one usually)
  content_len_pos = data.find_first_not_of(' ', content_len_pos);
  assert(content_len_pos < headers_end_pos);
  size_t content_length;
  auto [_, err] =
      std::from_chars(data.data() + content_len_pos, data.data() + headers_end_pos, content_length);
  if (err != std::errc{})
    throw http_exception("[http] cannot parse content length from server response: {}",
                         std::make_error_code(err).message());

  headers_end_pos += headers_end_marker.size();
  size_t overriden_bytes = data.size() - headers_end_pos;
  assert(overriden_bytes <= content_length);
  // remove headers, keep only overreaded body
  data.erase(data.begin(), data.begin() + headers_end_pos);
  data.resize(content_length);
  transfered = co_await async_read(
      socket, asio::buffer(data.data() + overriden_bytes, content_length - overriden_bytes), ec);
  if (ec)
    throw http_exception("[http] error while reading body: {}", ec.message());
  assert(transfered == content_length - overriden_bytes);
  drop_socket.no_longer_needed();
  co_return data;
}

}  // namespace tgbm
