
#include <cstdlib>
#include <thread>

#include <tgbm/bot.hpp>
#include <tgbm/logger.hpp>
#include <tgbm/net/http2/client.hpp>
#include <tgbm/net/http2/server.hpp>
#include <tgbm/utils/formatters.hpp>
#include <tgbm/net/asio/asio_tls_transport.hpp>

inline int handled_req_count = 0;

constexpr std::string_view expected_response = R"(
{
  "ok": true,
  "result": {
    "id": 123456789,
    "is_bot": true,
    "first_name": "MyBot",
    "username": "MyBotUsername",
    "can_join_groups": true,
    "can_read_all_group_messages": false,
    "supports_inline_queries": false
  }
}
)";

// empty json object, get method
constexpr std::string_view expected_getme_request = "{}";

// answers only getMe()
struct print_server : tgbm::http2_server {
  using tgbm::http2_server::http2_server;

  dd::task<tgbm::http_response> handle_request(tgbm::http_request req) {
    tgbm::http_response& rsp = co_await dd::this_coro::return_place;
    rsp.status = 200;
    rsp.body.insert(rsp.body.end(), expected_response.begin(), expected_response.end());
    auto& data = req.body.data;
    if (expected_getme_request != std::string_view((const char*)data.data(), data.size()))
      std::exit(3);
    ++handled_req_count;
    co_return dd::rvo;
  }
};

// ########################################## NOT SERVER ################################

dd::task<void> get_bot_info(tgbm::bot& b) {
  auto info = co_await b.api.getMe();
  if (info.id.value != 123456789 || info.is_bot != true || info.added_to_attachment_menu ||
      info.first_name != "MyBot" || info.username != "MyBotUsername" || info.can_join_groups != true ||
      info.can_read_all_group_messages != false || info.supports_inline_queries != false || info.is_premium) {
    TGBM_LOG_ERROR("expected bot info: {}, received: {}", expected_response, info);
    exit(10);
  }
}

int main() try {
  std::thread([] {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    TGBM_LOG_ERROR("timeout!");
    std::exit(4);
  }).detach();

  tgbm::tcp_connection_options opts{
      .disable_ssl_certificate_verify = true,
  };
  auto client = std::unique_ptr<tgbm::http_client>(
      new tgbm::http2_client("127.0.0.1",
                             {
                                 // disable timeouts, so client will not check them in run_one(500ms) later
                                 .timeout_check_interval = tgbm::duration_t::max(),
                             },
                             $inplace(tgbm::asio_tls_transport(std::move(opts)))));
  tgbm::bot bot("123456789:ABCDefghIJKlmnoPQRstuvWXyz1234567890", std::move(client));

  auto tf = $inplace(tgbm::asio_server_tls_transport(TEST_SERVER_CERT_PATH, TEST_SERVER_KEY_PATH));
  tgbm::http2_server_ptr server = new print_server(std::move(tf));

  namespace asio = boost::asio;
  asio::ip::tcp::endpoint ipv4_endpoint(asio::ip::address_v4::loopback(), 443);
  server->listen(ipv4_endpoint);

  asio::ip::tcp::endpoint ipv6_endpoint(asio::ip::address_v6::loopback(), 443);
  server->listen(ipv6_endpoint);

  server->start();

  for (int i = 0; i < 2; ++i)
    get_bot_info(bot).start_and_detach();

  using namespace std::chrono_literals;

  while (bot.get_client().run_one(250ms))
    ;
  // in the end only connection reader tries to read something and it will not be done
  // in 250ms (since no one sends data)
  if (handled_req_count != 2) {
    TGBM_LOG_ERROR("incorrect amount of request handled! Expected 2, actual: {}", handled_req_count);
    return 1;
  }
  return 0;
} catch (...) {
  TGBM_LOG_ERROR("unhandled exception!");
  return -1;
}
