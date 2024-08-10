#include "tgbot/net/BoostHttpOnlySslClient.h"

#include <boost/asio/ssl.hpp>

#include <cstddef>
#include <iostream>  // TODO rm
#include <vector>

namespace tgbm {

BoostHttpOnlySslClient::BoostHttpOnlySslClient() : _httpParser() {
}

BoostHttpOnlySslClient::~BoostHttpOnlySslClient() {
}

template <typename T, typename CallbackUser>
struct asio_awaiter {
  static_assert(std::is_same_v<T, std::decay_t<T>>);
  // also should not use its memory after passing callback to asio!
  static_assert(std::is_trivially_destructible_v<CallbackUser>);

  using ec_and_data = std::pair<boost::system::error_code, T>;
  union {
    CallbackUser _cb_user;
    ec_and_data _data;
  };
  explicit asio_awaiter(CallbackUser usr) noexcept : _cb_user(std::move(usr)) {
  }
  ~asio_awaiter() {
    // noop, if value exist, it was destroyed in await_resume
  }
  static bool await_ready() noexcept {
    return false;
  }

  void await_suspend(std::coroutine_handle<> handle) noexcept {
    auto cb = [this, handle](const boost::system::error_code& ec, T data) {
      std::construct_at(&_data, ec, std::move(data));
      std::cout << "START AGAIN" << std::this_thread::get_id() << std::endl;
      handle.resume();
    };
    std::cout << "START 1 " << std::this_thread::get_id() << std::endl;
    // assume does not use its memory after passing callback to asio
    _cb_user(cb);
    std::cout << "START 2" << std::endl;
  }
  [[nodiscard]] ec_and_data await_resume() noexcept {
    auto result = std::move(_data);
    std::destroy_at(&_data);
    return result;  // nrvo
  }
};

template <typename CallbackUser>
struct asio_awaiter<void, CallbackUser> {
  // also should not use its memory after passing callback to asio!
  static_assert(std::is_trivially_destructible_v<CallbackUser>);

  union {
    CallbackUser _cb_user;
    boost::system::error_code _ec;
  };
  explicit asio_awaiter(CallbackUser usr) noexcept : _cb_user(std::move(usr)) {
  }

  static bool await_ready() noexcept {
    return false;
  }

  void await_suspend(std::coroutine_handle<> handle) noexcept {
    auto cb = [this, handle](const boost::system::error_code& ec) {
      std::construct_at(&_ec, ec);
      std::cout << "START AGAIN " << std::this_thread::get_id() << std::endl;
      handle.resume();
    };
    // TODO почему-то как будто не останавливается...
    // assume does not use its memory after passing callback to asio
    std::cout << "START 1 " << std::this_thread::get_id() << std::endl;
    _cb_user(cb);
    std::cout << "START 2" << std::endl;
  }
  [[nodiscard]] boost::system::error_code await_resume() noexcept {
    return std::move(_ec);
  }
};

template <typename Protocol>
KELCORO_CO_AWAIT_REQUIRED auto async_resolve(
    boost::asio::ip::basic_resolver<Protocol>& resolver,
    std::type_identity_t<boost::asio::ip::basic_resolver_query<Protocol>> query) {
  auto cb_usr = [&]<typename T>(T&& cb) { resolver.async_resolve(std::move(query), std::forward<T>(cb)); };
  using resolve_results_t = typename boost::asio::ip::basic_resolver<Protocol>::results_type;
  return asio_awaiter<resolve_results_t, decltype(cb_usr)>(cb_usr);
}

template <typename Protocol>
KELCORO_CO_AWAIT_REQUIRED auto async_connect(
    boost::asio::basic_socket<Protocol>& socket,
    std::type_identity_t<typename boost::asio::ip::basic_resolver<Protocol>::results_type> endpoints) {
  auto cb_usr = [&]<typename T>(T&& cb) {
    boost::asio::async_connect(socket, endpoints, std::forward<T>(cb));
  };
  return asio_awaiter<boost::asio::ip::basic_endpoint<Protocol>, decltype(cb_usr)>(cb_usr);
}

template <typename Stream>
KELCORO_CO_AWAIT_REQUIRED auto async_handshake(boost::asio::ssl::stream<Stream>& stream,
                                               boost::asio::ssl::stream_base::handshake_type type) {
  auto cb_usr = [&]<typename T>(T&& cb) { stream.async_handshake(type, std::forward<T>(cb)); };
  return asio_awaiter<void, decltype(cb_usr)>(cb_usr);
}

KELCORO_CO_AWAIT_REQUIRED auto async_write(auto& stream, const auto& buffer) {
  auto cb_usr = [&]<typename T>(T&& cb) { boost::asio::async_write(stream, buffer, std::forward<T>(cb)); };
  return asio_awaiter<size_t, decltype(cb_usr)>(cb_usr);
}

template <typename Buf>
KELCORO_CO_AWAIT_REQUIRED auto async_read(auto& stream, Buf&& buffer) {
  auto cb_usr = [&]<typename T>(T&& cb) {
    boost::asio::async_read(stream, std::forward<Buf>(buffer), std::forward<T>(cb));
  };
  return asio_awaiter<size_t, decltype(cb_usr)>(cb_usr);
}

dd::task<std::string> BoostHttpOnlySslClient::makeRequest(const Url& url,
                                                          const std::vector<HttpReqArg>& args) const {
  namespace asio = boost::asio;
  namespace ssl = asio::ssl;

  using tcp = asio::ip::tcp;
  tcp::resolver resolver(_ioService);
  // TODO мне пока нужны минимальные sendMessage и как то получать в ответ сообщения
  // плюс создавать кнопки и inline query
  ssl::context context(ssl::context::tlsv12_client);
  context.set_default_verify_paths();

  ssl::stream<tcp::socket> socket(_ioService, context);
  // TODO ?? (хотя вроде нахуй не нужно, я итак узнаю что оно отвалилось т.к. постоянно посылаю телеграму что
  // то) socket.lowest_layer().set_option(boost::asio::socket_base::keep_alive(true));
  // TODO (изменил здесь) для коннекта нужен только ? https:// ? api.telegram.org
  tcp::resolver::query query(url.host, "443");
  boost::system::error_code ec;
  auto results = resolver.resolve(std::move(query), ec);
  // auto [ec, results] = co_await async_resolve(resolver, std::move(query));
  //  ДА КАК Я ОКАЗЫВАЮСЬ ЗДЕСЬ ТО БЛЕТ
  if (ec) {
    // TODO smth ? ? ? exception ? (terminate...)
    std::cerr << ec.message() << std::endl;
    co_return "";
  }
  asio::connect(socket.lowest_layer(), results, ec);
  // tcp::endpoint e;
  // std::tie(ec, e) = co_await async_connect(socket.lowest_layer(), results);
  if (ec) {
    // TODO smth
    std::cerr << ec.message() << std::endl;
    co_return "";
  }
#ifdef TGBOT_DISABLE_NAGLES_ALGORITHM
  socket.lowest_layer().set_option(tcp::no_delay(true));
#endif  // TGBOT_DISABLE_NAGLES_ALGORITHM

  // TODO подобрать хорошие значения
  socket.lowest_layer().set_option(asio::socket_base::send_buffer_size(32768));
  socket.lowest_layer().set_option(asio::socket_base::receive_buffer_size(32768));
  socket.set_verify_mode(ssl::verify_none);
  socket.set_verify_callback(ssl::rfc2818_verification(url.host));
  socket.handshake(ssl::stream_base::handshake_type::client);
  // ec = co_await async_handshake(socket, ssl::stream_base::handshake_type::client);
  if (ec) {
    // TODO smth
    std::cerr << ec.message() << std::endl;
    co_return "";
  }
  // END SOCKET CREATION (TODO into other function ::prepare or smth)

  // TODO доделать write/read
  // TODO проверить какой протокол, т.к. в http версии 1.0 соединение завершается сервером
  // а нужно хотя бы 1.1 и в общем надо смотреть тг какие умеет вообще
  std::string requestText = _httpParser.generateRequest(url, args, false);
  size_t transfered = 0;
  std::tie(ec, transfered) = co_await async_write(socket, asio::buffer(requestText));
  if (ec) {
    // TODO smth
    std::cerr << ec.message() << std::endl;
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
    std::tie(ec, transfered) = co_await async_read(socket, asio::buffer(buff));
    response += std::string_view(buff, transfered);
  }
  // TODO как хуёво, тут ещё и копирование строки
  // TODO просто сразу читать до \r\n\r\n и после этого читать тело. Тогда оно сразу будет у меня
  // TODO нужно чекнуть, что Content-Length вообще указывается телеграммом (wireshark...)
  co_return _httpParser.extractBody(response);
}

}  // namespace tgbm
