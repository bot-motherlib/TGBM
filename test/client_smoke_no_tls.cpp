#include <tgbm/net/http2/client.hpp>
#include <tgbm/net/asio/asio_transport.hpp>
#include <tgbm/net/http2/server.hpp>

constexpr std::string_view expected_response = "hello world";

// all noinlines here is workaround gcc-12 bug (miscompilation)

TGBM_GCC_WORKAROUND void fail(int i) {
  std::exit(i);
}

TGBM_GCC_WORKAROUND tgbm::http_response answer_req(tgbm::http_request req) {
  tgbm::http_response rsp;
  if (req.path == "/abc" && req.method == tgbm::http_method_e::GET) {
    rsp.status = 200;
    rsp.headers.push_back(tgbm::http_header_t{.name = "content-type", .value = "text/plain"});
    rsp.body.insert(rsp.body.end(), expected_response.begin(), expected_response.end());
    if (!req.body.data.empty()) {
      TGBM_LOG_ERROR("incorrect body, size: {}", req.body.data.size());
      fail(1);
    }
    return rsp;
  } else {
    TGBM_LOG_ERROR("incorrect path! Path is: {}", req.path);
    fail(2);
    return rsp;
  }
}

struct test_server : tgbm::http2_server {
  using tgbm::http2_server::http2_server;

  dd::task<tgbm::http_response> handle_request(tgbm::http_request req) {
    tgbm::http_response rsp = answer_req(std::move(req));
    co_return rsp;
  };
};

inline bool all_good = false;

TGBM_GCC_WORKAROUND dd::task<tgbm::http_response> make_test_request(tgbm::http2_client& client) {
  tgbm::http_request req{
      .path = "/abc",
      .method = tgbm::http_method_e::GET,
  };
  return client.send_request(std::move(req), tgbm::deadline_t::never());
}

TGBM_GCC_WORKAROUND void check_response(tgbm::http2_client& client, const tgbm::http_response& rsp) {
  if (rsp.headers.size() != 1) {  // status and content-type
    TGBM_LOG_ERROR("incorrect count of headers: {}", rsp.headers.size());
    fail(3);
  }
  if (rsp.headers[0].name != "content-type" || rsp.headers[0].value != "text/plain") {
    TGBM_LOG_ERROR("incorrect header: {}, value: {}", rsp.headers[0].name, rsp.headers[0].value);
    fail(4);
  }
  if (!std::ranges::equal(expected_response, rsp.body)) {
    TGBM_LOG_ERROR("incorrect body");
    fail(5);
  }
  all_good = true;
  TGBM_LOG_INFO("success");
}

dd::task<void> main_coro(tgbm::http2_client& client) {
  dd::task test_req = make_test_request(client);
  tgbm::http_response rsp = co_await test_req;
  check_response(client, rsp);
  client.stop();
}

int main() try {
  std::thread([] {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    TGBM_LOG_ERROR("timeout!");
    fail(4);
  }).detach();

  namespace asio = boost::asio;

  tgbm::http2_client_options opts{
      .timeout_check_interval = tgbm::duration_t::max(),  // disable timeouts
  };
  auto transport = $inplace(tgbm::asio_transport());
  tgbm::http2_client client(asio::ip::address_v6::loopback().to_string(), std::move(opts),
                            std::move(transport));

  auto tf = $inplace(tgbm::asio_server_transport(tgbm::tcp_connection_options{}, /*listen_thread_count=*/1));
  tgbm::http2_server_ptr server = new test_server(std::move(tf));

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
