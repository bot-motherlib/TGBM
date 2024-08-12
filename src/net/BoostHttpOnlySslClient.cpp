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
  explicit asio_awaiter(io_error_code& ec [[clang::lifetimebound]], Args&&... args) noexcept
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
  [[nodiscard]] T await_resume() noexcept {
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

// TODO delim тоже нужно захавтывать, но не выражается, надо через шаблонный арг
template <typename Stream, typename Buffer, std::convertible_to<std::string_view> StringView>
KELCORO_CO_AWAIT_REQUIRED [[clang::always_inline]] auto async_read_until(
    Stream& stream KELCORO_LIFETIMEBOUND, Buffer&& buffer KELCORO_LIFETIMEBOUND, StringView&& delim,
    io_error_code& ec KELCORO_LIFETIMEBOUND) {
  static_assert(!std::is_rvalue_reference_v<StringView&&>,
                "string delimiter will die before operation complete");
  return asio_awaiter<size_t, read_until_operation<Stream, std::remove_reference_t<Buffer>>>(
      ec, stream, buffer, std::string_view(delim));
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

dd::task<BoostHttpOnlySslClient::connection_t> BoostHttpOnlySslClient::create_connection(
    boost::asio::io_context& io_ctx, std::string host) {
  namespace asio = boost::asio;
  namespace ssl = asio::ssl;
  using tcp = asio::ip::tcp;

  tcp::resolver resolver(io_ctx);
  ssl::context context(ssl::context::tlsv12_client);
  context.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 |
                      ssl::context::single_dh_use);
  context.set_default_verify_paths();
  ssl::stream<tcp::socket> socket(io_ctx, context);
  tcp::resolver::query query(std::string(host), "https");
  io_error_code ec;
  auto results = co_await async_resolve(resolver, query, ec);
  if (ec) {
    LOG_ERR("[http] cannot resolve host: {}: service: {}, err: {}", query.host_name(), query.service_name(),
            ec.message());
    co_return {std::move(socket)};
  }
  asio::connect(socket.lowest_layer(), results, ec);
  tcp::endpoint e = co_await async_connect(socket.lowest_layer(), results, ec);
  if (ec) {
    LOG_ERR("[http] cannot connect to {}, err: {}", host, ec.message());
    co_return {std::move(socket)};
  }
  // TODO ? is needed?
  socket.lowest_layer().set_option(tcp::no_delay(true));

  // TODO подобрать хорошие значения
  socket.lowest_layer().set_option(asio::socket_base::send_buffer_size(32768));
  socket.lowest_layer().set_option(asio::socket_base::receive_buffer_size(32768));
  socket.set_verify_mode(ssl::verify_none);
  socket.set_verify_callback(ssl::host_name_verification(host));

  co_await async_handshake(socket, ssl::stream_base::handshake_type::client, ec);
  if (ec) {
    LOG_ERR("[http] cannot ssl handshake: {}", ec.message());
    co_return {std::move(socket)};
  }
  co_return {std::move(socket)};
}

dd::task<std::string> BoostHttpOnlySslClient::makeRequest(Url url, std::vector<HttpReqArg> args) {
  namespace asio = boost::asio;

  using tcp = asio::ip::tcp;
  auto handle = co_await connections.borrow();
  auto& socket = handle.get()->socket;
  LOG("just borrowed {}", (void*)&socket);
  // TODO remove bool flag (its debug)
  if (handle.get()->used) {
    LOG_ERR("{} ALREADY USED", (void*)&socket);
  }
  assert(!handle.get()->used);
  handle.get()->used = true;
  on_scope_exit {
    handle.get()->used = false;
    LOG("{} not used now", (void*)&socket);
  };
  on_scope_failure(drop_socket) {
    handle.drop();
  };
  // TODO RM h.drop();  // TODO rm, проверка мб пофиксит баги
  if (!socket.lowest_layer().is_open()) {
    LOG_ERR("[http] cannot {} socket connect to TG", (void*)&socket);
    // TODO drop and try again?
    co_return "";  // TODO error
  }
  io_error_code ec;
  std::string requestText = _httpParser.generateRequest(url, args, /*isKeepAlive=*/true);
  size_t transfered = co_await async_write(socket, asio::buffer(requestText), ec);
  if (ec) {
    LOG_ERR("[http] cannot write to {} socket, err: {}", (void*)&socket, ec.message());
    // TODO smth
    // std::cerr << ec.message() << std::endl;
    co_return "";
  }
  assert(transfered == requestText.size());
  // TODO timeout на весь сокет сразу одну штуку(точнее на одну операцию(корутину)...)

  std::string data;
  // TODO нужно возвращать вектор байт вместо строки 1. это выгоднее (sso не нужно)
  // 2. там могут быть \0, т.к. это в том числе загрузка файлов

  // read http headers
  // TODO тесты на конкретно эту часть кода
  std::string::size_type headers_end_pos = 0;
  static constexpr std::string_view headers_end_marker = "\r\n\r\n";
  static constexpr std::string_view content_length_header = "Content-Length:";
  // TODO видимо всё таки вынести в функцию read_until или типа того
  // TODO какая-то обработка на goto возможно этих еррор кодов, кек
  for (;;) {
    // TODO read http status string first, return http status code too
    // HTTP<version><whitespaces><code>
    // async read until may overread after \r\n\r\n
    char buf[128];
    transfered = co_await async_read_some(socket, asio::buffer(buf), ec);
    if (ec)
      break;
    data += std::string_view(buf, transfered);  // TODO better
    if (auto p = data.find(headers_end_marker, headers_end_pos); p != data.npos) {
      headers_end_pos = p;
      break;
    }
    headers_end_pos += transfered;
  }
  if (ec) {
    // TODO smth
    LOG_ERR("[http] error while reading http headers: {}", ec.message());
    co_return "";
  }
  auto content_len_pos = data.find(content_length_header);
  if (content_len_pos == data.npos) {
    LOG_ERR("[http] incorrect server answer");
    co_return "";
  }
  content_len_pos += content_length_header.size();
  // skip whitespaces (exactly one usually)
  content_len_pos = data.find_first_not_of(' ', content_len_pos);
  assert(content_len_pos < headers_end_pos);
  size_t content_length;
  auto [_, err] =
      std::from_chars(data.data() + content_len_pos, data.data() + headers_end_pos, content_length);
  if (err != std::errc{}) {
    // TODO smth, incorrect server answer ?
    LOG_ERR("[http] cannot parse content length from server response");
    co_return "";
  }
  headers_end_pos += headers_end_marker.size();
  size_t overriden_bytes = data.size() - headers_end_pos;
  assert(overriden_bytes <= content_length);
  // remove headers, keep only overreaded body
  data.erase(data.begin(), data.begin() + headers_end_pos);
  data.resize(content_length);
  // TODO что будет если надо прочитать 0 байт? Вернётся сразу asio или зависнет нахрен?
  transfered = co_await async_read(
      socket, asio::buffer(data.data() + overriden_bytes, content_length - overriden_bytes), ec);
  if (ec) {
    // TODO smth
    LOG_ERR("[http] error while reading body: {}", ec.message());
    co_return "";
  }
  assert(transfered == content_length - overriden_bytes);
  drop_socket.no_longer_needed();
  co_return data;
}

}  // namespace tgbm
