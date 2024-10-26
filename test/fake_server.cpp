#include "tgbm/net/http2_server.hpp"

#include <fmt/core.h>

struct print_server : tgbm::http2_server {
  using tgbm::http2_server::http2_server;

  dd::task<tgbm::http_response> handle_request(tgbm::http_request req) {
    tgbm::http_response& rsp = co_await dd::this_coro::return_place;
    rsp.status = 200;
    std::string answer = R"(
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
    rsp.body.insert(rsp.body.end(), answer.begin(), answer.end());
    fmt::println("request, path: {}, body {}", req.path,
                 std::string_view((const char*)req.body.data.data(), req.body.data.size()));
    co_return dd::rvo;
  }
};

namespace asio = boost::asio;

int main() {
  tgbm::http2_server_ptr server = new print_server(tgbm::http2_server_options{
      .ssl_cert_path = "E:/dev/ssl_test_crt/server.crt",
      .private_key_path = "E:/dev/ssl_test_crt/server.key",
  });

  asio::ip::tcp::endpoint ipv4_endpoint(asio::ip::address_v4::loopback(), 443);
  server->listen(ipv4_endpoint);

  asio::ip::tcp::endpoint ipv6_endpoint(asio::ip::address_v6::loopback(), 443);
  server->listen(ipv6_endpoint);

  server->run();
}
