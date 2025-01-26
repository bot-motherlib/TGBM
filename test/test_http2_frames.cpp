
#include <list>
#include <string>

#include <kelcoro/task.hpp>
#include <kelcoro/generator.hpp>

#include <tgbm/net/transport_factory.hpp>
#include <tgbm/net/http2/protocol.hpp>
#include <tgbm/net/http_body.hpp>
#include <tgbm/net/http2/client.hpp>
#include <tgbm/utils/algorithm.hpp>

#include <fmt/core.h>

#define REQUIRE(...)  \
  if (!(__VA_ARGS__)) \
  std::exit(__LINE__)

namespace {

struct test_factory;

struct test_connection {
  test_factory& factory;
  tgbm::bytes_t bytes_to_send;  // what connection will sent to client on 'read'
  tgbm::bytes_t tmp;            // what validator will see
  dd::generator<dd::nothing_t> validator;
  // not more that one reader at a time
  std::coroutine_handle<> reader;
  std::span<tgbm::byte_t> reader_buf;
  tgbm::io_error_code* reader_ec = nullptr;

  test_connection(test_factory& f, tgbm::fn_ref<dd::generator<dd::nothing_t>(test_connection&)> foo)
      : factory(f), validator(foo(*this)) {
  }

  test_connection(test_connection&&) = delete;
  void operator=(test_connection&&) = delete;

  // interface for validator
  void push_answer_bytes(tgbm::bytes_t to_sent);

  void start_read(std::coroutine_handle<> callback, std::span<tgbm::byte_t> buf, tgbm::io_error_code& ec);

  void start_write(std::coroutine_handle<> callback, std::span<const tgbm::byte_t> buf,
                   tgbm::io_error_code& ec, size_t& written);

  void shutdown() noexcept;
  static constexpr bool is_https = false;
};

struct test_connection_task {
  test_connection* con = nullptr;
  std::coroutine_handle<> handle;
  tgbm::io_error_code& ec;
};

struct test_factory {
 private:
  std::list<test_connection_task> queue;
  bool stop_requested = false;
  tgbm::move_only_fn<dd::generator<dd::nothing_t>(test_connection&)> validators_factory;

  friend struct test_connection;

 public:
  explicit test_factory(tgbm::move_only_fn<dd::generator<dd::nothing_t>(test_connection&)> f)
      : validators_factory(std::move(f)) {
    assert(validators_factory);
  }
  dd::task<tgbm::any_connection> create_connection(std::string_view /*host*/) {
    co_return $inplace(test_connection{*this, *&validators_factory});
  }
  void run() {
    stop_requested = false;
    while (!stop_requested && !queue.empty()) {
      auto x = std::move(queue.front());
      queue.pop_front();
      x.handle.resume();
    }
  }
  void stop() {
    stop_requested = true;
    while (!queue.empty()) {
      auto x = queue.front();
      x.ec = boost::asio::error::operation_aborted;
      queue.pop_front();
      x.handle.resume();
    }
  }
  bool run_one(tgbm::duration_t timeout) {
    stop_requested = false;
    REQUIRE(!queue.empty());
    auto x = std::move(queue.front());
    queue.pop_front();
    x.handle.resume();
    return true;
  }
  dd::task<void> sleep(tgbm::duration_t d, tgbm::io_error_code& ec) {
    throw std::runtime_error("must not be used");
  }
};

void test_connection::push_answer_bytes(tgbm::bytes_t to_sent) {
  bytes_to_send.insert(bytes_to_send.end(), to_sent.begin(), to_sent.end());
  if (reader && reader_buf.size() <= bytes_to_send.size()) {
    auto b = bytes_to_send.begin();
    auto e = b + reader_buf.size();
    std::copy(b, e, reader_buf.begin());
    bytes_to_send.erase(b, e);
    assert(reader_ec && reader);
    factory.queue.push_back({this, reader, *reader_ec});
    reader = nullptr;
    reader_buf = {};
    reader_ec = nullptr;
  }
}

void test_connection::start_read(std::coroutine_handle<> callback, std::span<tgbm::byte_t> buf,
                                 tgbm::io_error_code& ec) {
  if (reader)
    throw std::runtime_error("reader already exist! Test failed");
  if (bytes_to_send.size() >= buf.size()) {
    auto it = bytes_to_send.begin();
    auto e = it + buf.size();
    std::copy(it, e, buf.begin());
    bytes_to_send.erase(it, e);
    factory.queue.push_back(test_connection_task{this, callback, ec});
    return;
  }
  reader = callback;
  reader_buf = buf;
  reader_ec = &ec;
}

void test_connection::shutdown() noexcept {
  if (reader) {
    *reader_ec = boost::asio::error::operation_aborted;
    std::coroutine_handle r = reader;
    reader = nullptr;
    reader_ec = nullptr;
    reader_buf = {};
    r.resume();
  }
  while (!factory.queue.empty()) {
    auto [c, t, ec] = factory.queue.front();
    if (c == this) {
      ec = boost::asio::error::operation_aborted;
      t.resume();
    }
  }
}

void test_connection::start_write(std::coroutine_handle<> callback, std::span<const tgbm::byte_t> buf,
                                  tgbm::io_error_code& ec, size_t& written) {
  if (factory.stop_requested) {
    ec = boost::asio::error::operation_aborted;
    written = 0;
  } else {
    tmp.insert(tmp.end(), buf.begin(), buf.end());
    written = buf.size();
    factory.queue.push_back({nullptr, validator.raw_handle().promise().current_worker, ec});
  }
  callback.resume();
}

dd::generator<dd::nothing_t> wait_bytes_n(tgbm::bytes_t& input, size_t n) {
  while (input.size() < n)
    co_yield {};
}

void skip(tgbm::bytes_t& input, size_t n) {
  assert(n <= input.size());
  input.erase(input.begin(), input.begin() + n);
}
dd::generator<dd::nothing_t> wait_connection_preface(tgbm::bytes_t& input) {
  std::span preface = tgbm::http2::connection_preface;
  co_yield dd::elements_of(wait_bytes_n(input, preface.size()));
  REQUIRE(tgbm::starts_with(input, preface));
  skip(input, preface.size());
}

dd::generator<dd::nothing_t> wait_header_and_data(tgbm::bytes_t& input, tgbm::http2::frame_header& h,
                                                  tgbm::bytes_t& data) {
  while (input.size() < tgbm::http2::frame_header_len)
    co_yield {};
  h = tgbm::http2::frame_header::parse({input.data(), tgbm::http2::frame_header_len});
  while (input.size() < h.length + tgbm::http2::frame_header_len)
    co_yield {};
  data.clear();
  auto b = input.begin() + tgbm::http2::frame_header_len;
  data.insert(data.begin(), b, b + h.length);
  skip(input, tgbm::http2::frame_header_len + h.length);
}

dd::generator<dd::nothing_t> connection_validator(test_connection& con) {
  auto& input = con.tmp;
  co_yield dd::elements_of(wait_connection_preface(input));
  tgbm::http2::frame_header hdr;
  tgbm::bytes_t data;
  co_yield dd::elements_of(wait_header_and_data(input, hdr, data));
  REQUIRE(hdr.type == tgbm::http2::frame_e::SETTINGS && hdr.flags == 0 && hdr.stream_id == 0 &&
          (hdr.length % 6) == 0);
  tgbm::bytes_t bytes;
  tgbm::http2::settings_frame::form({}, std::back_inserter(bytes));
  tgbm::http2::accepted_settings_frame().send_to(std::back_inserter(bytes));
  con.push_answer_bytes(std::move(bytes));
  co_yield dd::elements_of(wait_header_and_data(input, hdr, data));
  REQUIRE(hdr == tgbm::http2::accepted_settings_frame());
  co_yield dd::elements_of(wait_header_and_data(input, hdr, data));
  REQUIRE(hdr.type == tgbm::http2::frame_e::HEADERS);
  co_yield dd::elements_of(wait_header_and_data(input, hdr, data));
  REQUIRE(hdr.type == tgbm::http2::frame_e::DATA);
  bytes.clear();
  // TODO check this test fully
  tgbm::http2::frame_header h;
  h.type = tgbm::http2::frame_e::HEADERS;
  h.stream_id = hdr.stream_id;
  h.flags = tgbm::http2::flags::END_HEADERS | tgbm::http2::flags::END_STREAM;
  h.length = 0;
  h.send_to(std::back_inserter(bytes));
  con.push_answer_bytes(bytes);

  fmt::println("received {}, {}", hdr.stream_id, (int)hdr.type);
}

void test1() {
  auto f = [](test_connection& con) { return connection_validator(con); };
  tgbm::http2_client client("api.telegram.org", {.timeout_check_interval = tgbm::duration_t::max()},
                            $inplace((test_factory(f))));
  client
      .send_request(tgbm::http_request{.path = "/", .method = tgbm::http_method_e::OPTIONS},
                    tgbm::deadline_t::never())
      .start_and_detach();
  client.run();
}

}  // namespace

int main() {
  test1();
  return 0;
}
