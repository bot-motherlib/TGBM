#include <tgbm/net/http2/client.hpp>
#include <tgbm/net/asio_transport.hpp>
#include <tgbm/net/http2/server.hpp>

constexpr std::string_view expected_response = "hello world";

struct test_server : tgbm::http2_server {
  using tgbm::http2_server::http2_server;

  dd::task<tgbm::http_response> handle_request(tgbm::http_request req) {
    tgbm::http_response& rsp = co_await dd::this_coro::return_place;
    if (req.path == "/abc" && req.method == tgbm::http_method_e::GET) {
      rsp.status = 200;
      rsp.headers.push_back(tgbm::http_header_t{.name = "content-type", .value = "text/plain"});
      rsp.body.insert(rsp.body.end(), expected_response.begin(), expected_response.end());
      if (!req.body.data.empty()) {
        TGBM_LOG_ERROR("incorrect body, size: {}", req.body.data.size());
        std::exit(1);
      }
      co_return dd::rvo;
    } else {
      TGBM_LOG_ERROR("incorrect path! Path is: {}", req.path);
      std::exit(2);
    }
  };
};

inline bool all_good = false;

dd::task<void> main_coro(tgbm::http2_client& client) {
  tgbm::http_response rsp = co_await client.send_request(
      {
          .path = "/abc",
          .method = tgbm::http_method_e::GET,
      },
      tgbm::deadline_after(std::chrono::seconds(120)));
  if (rsp.headers.size() != 1) {  // status and content-type
    TGBM_LOG_ERROR("incorrect count of headers: {}", rsp.headers.size());
    std::exit(3);
  }
  if (rsp.headers[0].name != "content-type" || rsp.headers[0].value != "text/plain") {
    TGBM_LOG_ERROR("incorrect header: {}, value: {}", rsp.headers[0].name, rsp.headers[0].value);
    std::exit(4);
  }
  if (!std::ranges::equal(expected_response, rsp.body)) {
    TGBM_LOG_ERROR("incorrect body");
    std::exit(5);
  }
  all_good = true;
  TGBM_LOG_INFO("success");
  client.stop();
}

int main() try {
  std::thread([] {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    TGBM_LOG_ERROR("timeout!");
    std::exit(4);
  }).detach();

  namespace asio = boost::asio;

  tgbm::http2_client client(asio::ip::address_v6::loopback().to_string(),
                            {.timeout_check_interval = tgbm::duration_t::max()},  // disable timeouts
                            tgbm::make_any_transport_factory<tgbm::asio_transport>());

  tgbm::http2_server_ptr server =
      new test_server(tgbm::make_any_server_transport_factory<tgbm::asio_server_transport>());

  asio::ip::tcp::endpoint ipv6_endpoint(asio::ip::address_v6::loopback(), 80);
  server->listen(ipv6_endpoint);

  server->start();

  main_coro(client).start_and_detach();
  client.run();

  if (!all_good)
    return 8;
  return 0;
} catch (...) {
  TGBM_LOG_ERROR("unknown error");
  return 9;
}
