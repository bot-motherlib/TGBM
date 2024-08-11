#include "tgbm/net/BoostHttpOnlySslClient.h"

#include <cstddef>
#include <vector>

#include <boost/asio.hpp>

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
    co_return socket;
  }
  asio::connect(socket.lowest_layer(), results, ec);
  // tcp::endpoint e;
  // std::tie(ec, e) = co_await async_connect(socket.lowest_layer(), results);
  if (ec) {
    // TODO smth
    // std::cerr << ec.message() << std::endl;
    co_return socket;
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
    co_return socket;
  }
  co_return socket;
}

dd::task<std::string> BoostHttpOnlySslClient::makeRequest(const Url& url,
                                                          const std::vector<HttpReqArg>& args) {
  namespace asio = boost::asio;

  using tcp = asio::ip::tcp;
  auto h = co_await connections.borrow();
  connection_t& socket = *h.get();
  if (!socket.lowest_layer().is_open())
    co_return "";  // TODO error
  h.drop();        // TODO коннекншн пол нахрен не нужен, если нет keep alive
  io_error_code ec;
  // TODO доделать write/read
  // TODO проверить какой протокол, т.к. в http версии 1.0 соединение завершается сервером
  // а нужно хотя бы 1.1 и в общем надо смотреть тг какие умеет вообще
  std::string requestText = _httpParser.generateRequest(url, args, false);  // TODO set keep alive true
  size_t transfered = co_await async_write(socket, asio::buffer(requestText), ec);
  if (ec) {
    // TODO smth
    // std::cerr << ec.message() << std::endl;
    co_return "";
  }
  assert(transfered == requestText.size());
  // TODO timeout

  std::string response;
  // TODO по другому читать как-то, учитывая что будет keep-alive (позже)
  char buff[4096];
  // TODO видимо здесь надеются, что соединение будет закрыто в другой стороны после отправки ответа
  // вероятно потому что отправляется .close вместо keep-alive!!! Это где-то напрямую отправляется!!!
  // TODO boost::asio::transfer_exactly(content_length)
  // TODO нужно возвращать вектор байт вместо строки 1. это выгоднее (sso не нужно)
  // 2. там могут быть \0, т.к. это в том числе загрузка файлов
  while (!ec) {
    transfered = co_await async_read(socket, asio::buffer(buff), ec);
    response += std::string_view(buff, transfered);
  }
  // TODO как хуёво, тут ещё и копирование строки
  // TODO просто сразу читать до \r\n\r\n и после этого читать тело. Тогда оно сразу будет у меня
  // TODO нужно чекнуть, что Content-Length вообще указывается телеграммом (wireshark...)
  co_return _httpParser.extractBody(response);
}

}  // namespace tgbm
