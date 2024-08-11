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
  // also should not use its memory after passing callback to asio!
  static_assert(std::is_trivially_destructible_v<CallbackUser>);

  union {
    CallbackUser _cb_user;
    T _data;
  };
  io_error_code& _ec;

  explicit asio_awaiter(CallbackUser usr, io_error_code& ec) noexcept : _cb_user(std::move(usr)), _ec(ec) {
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
    // assume does not use its memory after passing callback to asio
    _cb_user(cb);
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
  explicit asio_awaiter(CallbackUser usr, io_error_code& ec) noexcept : _cb_user(std::move(usr)), _ec(ec) {
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
KELCORO_CO_AWAIT_REQUIRED auto async_resolve(
    boost::asio::ip::basic_resolver<Protocol>& resolver,
    std::type_identity_t<boost::asio::ip::basic_resolver_query<Protocol>> query, io_error_code& ec) {
  auto cb_usr = [&]<typename T>(T&& cb) { resolver.async_resolve(std::move(query), std::forward<T>(cb)); };
  using resolve_results_t = typename boost::asio::ip::basic_resolver<Protocol>::results_type;
  return asio_awaiter<resolve_results_t, decltype(cb_usr)>(cb_usr, ec);
}

template <typename Protocol>
KELCORO_CO_AWAIT_REQUIRED auto async_connect(
    boost::asio::basic_socket<Protocol>& socket,
    std::type_identity_t<typename boost::asio::ip::basic_resolver<Protocol>::results_type> endpoints,
    io_error_code& ec) {
  auto cb_usr = [&]<typename T>(T&& cb) {
    boost::asio::async_connect(socket, endpoints, std::forward<T>(cb));
  };
  return asio_awaiter<boost::asio::ip::basic_endpoint<Protocol>, decltype(cb_usr)>(cb_usr, ec);
}

template <typename Stream>
KELCORO_CO_AWAIT_REQUIRED auto async_handshake(boost::asio::ssl::stream<Stream>& stream,
                                               boost::asio::ssl::stream_base::handshake_type type,
                                               io_error_code& ec) {
  auto cb_usr = [&]<typename T>(T&& cb) { stream.async_handshake(type, std::forward<T>(cb)); };
  return asio_awaiter<void, decltype(cb_usr)>(cb_usr, ec);
}

KELCORO_CO_AWAIT_REQUIRED auto async_write(auto& stream, const auto& buffer, io_error_code& ec) {
  auto cb_usr = [&]<typename T>(T&& cb) { boost::asio::async_write(stream, buffer, std::forward<T>(cb)); };
  return asio_awaiter<size_t, decltype(cb_usr)>(cb_usr, ec);
}

template <typename Buf>
KELCORO_CO_AWAIT_REQUIRED auto async_read(auto& stream, Buf&& buffer, io_error_code& ec) {
  auto cb_usr = [&]<typename T>(T&& cb) {
    boost::asio::async_read(stream, std::forward<Buf>(buffer), std::forward<T>(cb));
  };
  return asio_awaiter<size_t, decltype(cb_usr)>(cb_usr, ec);
}

template <typename Buf>
KELCORO_CO_AWAIT_REQUIRED auto async_read_some(auto& stream, Buf&& buffer, io_error_code& ec) {
  auto cb_usr = [&]<typename T>(T&& cb) {
    stream.async_read_some(std::forward<Buf>(buffer), std::forward<T>(cb));
  };
  return asio_awaiter<size_t, decltype(cb_usr)>(cb_usr, ec);
}

template <typename Buf>
KELCORO_CO_AWAIT_REQUIRED auto async_read_until(auto& stream, Buf&& buffer, std::string_view delim,
                                                io_error_code& ec) {
  auto cb_usr = [&]<typename T>(T&& cb) {
    boost::asio::async_read_until(stream, std::forward<Buf>(buffer), delim, std::forward<T>(cb));
  };
  return asio_awaiter<size_t, decltype(cb_usr)>(cb_usr, ec);
}

template <typename Buf>
KELCORO_CO_AWAIT_REQUIRED auto async_read_exactly(auto& stream, Buf&& buffer, size_t count,
                                                  io_error_code& ec) {
  auto cb_usr = [&]<typename T>(T&& cb) {
    boost::asio::async_read(stream, std::forward<Buf>(buffer), boost::asio::transfer_exactly(count),
                            std::forward<T>(cb));
  };
  return asio_awaiter<size_t, decltype(cb_usr)>(cb_usr, ec);
}

dd::task<BoostHttpOnlySslClient::connection_t> BoostHttpOnlySslClient::create_connection(
    boost::asio::io_context& io_ctx, std::string host) {
  namespace asio = boost::asio;
  namespace ssl = asio::ssl;
  using tcp = asio::ip::tcp;
  tcp::resolver resolver(io_ctx);
  ssl::context context(ssl::context::tlsv12_client);
  context.set_default_verify_paths();
  ssl::stream<tcp::socket> socket(io_ctx, context);
  // TODO ?? (хотя вроде нахуй не нужно, я итак узнаю что оно отвалилось т.к. постоянно посылаю телеграму что
  // то) socket.lowest_layer().set_option(boost::asio::socket_base::keep_alive(true));
  tcp::resolver::query query(std::string(host), "https");
  io_error_code ec;
  auto results = resolver.resolve(std::move(query), ec);
  // auto [ec, results] = co_await async_resolve(resolver, std::move(query));
  if (ec) {
    // TODO smth ? ? ? exception ? (terminate...)
    // std::cerr << ec.message() << std::endl;
    co_return {std::move(socket)};
  }
  asio::connect(socket.lowest_layer(), results, ec);
  // tcp::endpoint e;
  // std::tie(ec, e) = co_await async_connect(socket.lowest_layer(), results);
  if (ec) {
    // TODO smth
    // std::cerr << ec.message() << std::endl;
    co_return {std::move(socket)};
  }
  // TODO ? is needed?
  socket.lowest_layer().set_option(tcp::no_delay(true));

  // TODO подобрать хорошие значения
  socket.lowest_layer().set_option(asio::socket_base::send_buffer_size(32768));
  socket.lowest_layer().set_option(asio::socket_base::receive_buffer_size(32768));
  socket.set_verify_mode(ssl::verify_none);
  socket.set_verify_callback(ssl::host_name_verification(host));
  socket.handshake(socket.client);
  // ec = co_await async_handshake(socket, ssl::stream_base::handshake_type::client);
  if (ec) {
    // TODO smth
    // std::cerr << ec.message() << std::endl;
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
  for (;;) {
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
