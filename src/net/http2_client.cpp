/*
possible situatuions:
  1. connection drop (must release all resources)
  2. stop (goes into connection drop) and must make client ready to .run again

resources:
1. requests
    request borrows connection or wait in client.connection_waiters until connection done
  * send_request creates request_node
  * node -> into connection->requests
  - if node ignores headers and data, owner will own node and finish it after sending data (or error)
  - otherwise, node -> responses, writer will write its request
  * reader read headers/data and fills request, finishes request on STREAM_END
  * timeout/cancel(.destroy) node on any step unlinks it
  ALL requests are ended by connection.finish_request(node, status), except node is .destroyed

2. connection
    There are 0 or 1 connection at one time,
    connection initiated when request handled and no active connection or connection if out of stream ids
    (reader handles all pending streams and goes to create new connection if someone waits it)

   connection may be dropped only by its reader/writer or when stop requested

3. connection_waiters:
    * drop_connection() -> nothing
    * cleared on the end of start_connecting (on success or error)
    * stop() -> forbids creation of new connections, drops connection, resumes all connection_waiters
    without results (they will throw)

4. Timers:
   4.1. request timers
      each request linked to connection.timers multiset and erased from it when
       * finished normaly
       * finished by timeout (checked in .run())
       * drop_connection() / stop() -> finish_all_with_exception which erases from timers
   Note: now its possible for request to be handled longer then timeout, if this request
   was creating connection. After creation he will be canceled, but connection will still exist

*/

#include "boost/smart_ptr/intrusive_ptr.hpp"
#include "tgbm/net/http2/hpack.hpp"
#include "tgbm/net/http2/protocol.hpp"
#include "tgbm/net/http2_client.hpp"
#include "tgbm/net/asio_awaiters.hpp"
#include "tgbm/logger.h"
#include "tgbm/tools/scope_exit.h"
#include "tgbm/tools/macro.hpp"
#include "tgbm/net/errors.hpp"
#include "tgbm/tools/reusable_buffer.hpp"
#include "tgbm/tools/deadline.hpp"

#include <fmt/ranges.h>

#include <kelcoro/channel.hpp>

#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/treap_set.hpp>
#include <boost/intrusive/list_hook.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/slist.hpp>

namespace tgbm {

constexpr inline auto h2fhl = http2::frame_header_len;

struct http2_frame_buffer {
  byte_t* b;
  byte_t* m;
  byte_t* e;

  explicit http2_frame_buffer(std::span<byte_t> buf) : b(buf.data()), m(b), e(buf.data() + buf.size()) {
  }

  [[nodiscard]] size_t available() const noexcept {
    return std::distance(m, e);
  }
  // how many can be filled with frame_header in mind
  [[nodiscard]] size_t available_payload() const noexcept {
    size_t avail = available();
    return avail < h2fhl ? 0 : avail - h2fhl;
  }

  // must be called only once on one buffer
  [[nodiscard]] std::span<byte_t> filled_bytes() const noexcept {
    return std::span<byte_t>(b, m);
  }

  [[nodiscard]] bool push_frame(http2::frame_header header, std::span<byte_t> payload) noexcept {
    assert(header.length == payload.size());
    if (available() < h2fhl + payload.size())
      return false;
    m = header.send_to(m);
    m = copy_bytes(payload, m);
    return true;
  }
  [[nodiscard]] bool push_frame(http2::frame_header header, byte_t* payload, size_t payload_len) noexcept {
    return push_frame(header, std::span<byte_t>(payload, payload_len));
  }
  // assumes payload have atleast header.length bytes
  [[nodiscard]] bool push_frame(http2::frame_header header, byte_t* payload) noexcept {
    return push_frame(header, std::span<byte_t>(payload, header.length));
  }
};

struct http2_frame_t {
  http2::frame_header header;
  std::span<byte_t> data;
};

struct prepared_http_request {
  http2::stream_id_t streamid = 0;
  bytes_t headers;
  bytes_t data;
};

struct request_node;

using node_ptr = boost::intrusive_ptr<request_node>;

static bytes_t generate_http2_headers(const http_request& request, hpack::encoder<>& encoder,
                                      std::string_view host) {
  using hdrs = hpack::static_table_t::values;

  assert(!request.path.empty());
  assert(!(request.body.content_type.empty() && !request.body.data.empty()));

  bytes_t headers;
  headers.reserve(128);
  auto out = std::back_inserter(headers);

  // required scheme, method, authority, path

  encoder.encode_header_fully_indexed(hdrs::scheme_https, out);
  switch (request.method) {
    case http_method_e::GET:
      encoder.encode_header_fully_indexed(hdrs::method_get, out);
      break;
    case http_method_e::POST:
      encoder.encode_header_fully_indexed(hdrs::method_post, out);
      break;
    default:
      encoder.encode_header_and_cache(hdrs::method_get, e2str(request.method), out);
  }
  encoder.encode_header_memory_effective<true>(hdrs::authority, host, out);
  encoder.encode_header_memory_effective<true, true>(hdrs::path, request.path, out);
  encoder.encode_header_never_indexing(hdrs::user_agent, "kelbon", out);
  // content type and custom headers

  if (!request.body.data.empty())
    encoder.encode_header_memory_effective<true>(hdrs::content_type, request.body.content_type, out);
  for (auto& [name, value] : request.headers)
    encoder.encode_header_memory_effective<true>(name, value, out);
  return headers;
}

static prepared_http_request form_http2_request(http_request&& request, http2::stream_id_t streamid,
                                                hpack::encoder<>& encoder, std::string_view host) {
  return prepared_http_request{
      .streamid = streamid,
      .headers = generate_http2_headers(request, encoder, host),
      .data = std::move(request.body.data),
  };
}

// request starts in connection.requests, then goes into connection.responses
struct request_node {
  // used when request started in connection.requests and when its free in connection.free_nodes
  bi::list_member_hook<link_option> requests_hook;
  bi::unordered_set_member_hook<link_option> responses_hook;
  bi::bs_set_member_hook<link_option> timers_hook;
  uint32_t refcount = 0;
  uint32_t streamlevel_window_size = 0;
  // 'new_request_node' fills req, deadline, writer_handle and streamlevel_window_size
  prepared_http_request req;  // will be moved into writer
  deadline_t deadline;
  union {
    std::coroutine_handle<> writer_handle = {};  // setted initialy before 'await_suspend'
    dd::task<int>::handle_type task;             // setted by 'await_suspend' (requester)
  };
  http2_connection_ptr connection = nullptr;
  // received resonse (filled by 'reader' in connection)
  on_header_fn_ptr on_header;
  on_data_part_fn_ptr on_data_part;
  int status = reqerr_e::unknown_err;
  TGBM_PIN;

  // precondition: started ()
  [[nodiscard]] bool finished() const noexcept {
    return task == nullptr;
  }
  static bool await_ready() noexcept {
    return false;
  }
  std::coroutine_handle<> await_suspend(dd::task<int>::handle_type h) noexcept {
    // get union member and replace by another handle
    std::coroutine_handle writer_h = writer_handle;
    task = h;
    if (writer_h)  // if writer suspended now
      return writer_h;
    return std::noop_coroutine();
  }

  [[nodiscard]] int await_resume() const noexcept {
    return status;
  }

  [[nodiscard]] bool ignores_response() const noexcept {
    return !on_header && !on_data_part;
  }

  // returns false on protocol errors
  // precondition: padding removed
  void receive_headers(hpack::decoder<>& decoder, http2_frame_t&& frame) {
    assert(frame.header.stream_id == req.streamid);
    assert(frame.header.type == http2::frame_e::HEADERS);
    // weird things like continuations, trailers, many header frames with CONTINUE etc not supported
    if (!(frame.header.flags & http2::flags::END_HEADERS))
      throw protocol_error{};
    const byte_t* in = frame.data.data();
    const byte_t* e = in + frame.data.size();
    status = decoder.decode_response_status(in, e);
    // TODO lazy headers, then try parse first header(status), if cannot, then not set
    // note: its possible if i ignore headers, that 'status' will be cached and dynamic table broken
    // but it must be very strange case where implementation caches status
    hpack::decode_headers_block(decoder, std::span(in, e),
                                [&](std::string_view name, std::string_view value) {
                                  if (on_header)
                                    (*on_header)(name, value);
                                });
  }

  // returns false on protocol errors
  // precondition: padding removed
  void receive_data(http2_frame_t&& frame, bool end_stream) {
    assert(frame.header.stream_id == req.streamid);
    assert(frame.header.type == http2::frame_e::DATA);
    if (on_data_part)
      (*on_data_part)(frame.data, end_stream);
  }
  struct equal_by_streamid {
    bool operator()(const http2::stream_id_t& l, const http2::stream_id_t& r) const noexcept {
      return l == r;
    }
  };
  struct key_of_value {
    using type = http2::stream_id_t;
    const type& operator()(const request_node& v) const noexcept {
      return v.req.streamid;
    }
  };
  struct hash_by_streamid {
    size_t operator()(http2::stream_id_t s) const noexcept {
      // only grow (1, 3, 5...) so last bit is always 1, never intersects
      // uint32_t, so always less uint32_t max (high bits always false and fixable by |, but i dont need it)
      // proved as best possible hash for this case in bench
      return s >> 1;
    }
  };
  struct compare_by_deadline {
    bool operator()(const request_node& l, const request_node& r) const noexcept {
      return l.deadline < r.deadline;  // less means higher priority
    }
  };
};

dd::task<void> send_rst_stream(http2_connection_ptr con, http2::stream_id_t streamid, http2::errc_e errc);

struct http2_connection {
  using requests_member_hook =
      bi::member_hook<request_node, bi::list_member_hook<link_option>, &request_node::requests_hook>;
  using responses_member_hook = bi::member_hook<request_node, bi::unordered_set_member_hook<link_option>,
                                                &request_node::responses_hook>;
  using requests_t = bi::list<request_node, bi::cache_last<true>, requests_member_hook>;
  using responses_t = bi::unordered_set<request_node, bi::constant_time_size<true>, responses_member_hook,
                                        bi::key_of_value<request_node::key_of_value>,
                                        bi::equal<request_node::equal_by_streamid>,
                                        bi::hash<request_node::hash_by_streamid>, bi::power_2_buckets<true>>;
  using timers_member_hook =
      bi::member_hook<request_node, bi::bs_set_member_hook<link_option>, &request_node::timers_hook>;
  using timers_t = bi::treap_multiset<request_node, bi::constant_time_size<true>, timers_member_hook,
                                      bi::priority<request_node::compare_by_deadline>,
                                      bi::compare<request_node::compare_by_deadline>>;
  http2::settings_t server_settings;
  http2::settings_t client_settings;
  tcp_connection asio_con;
  hpack::encoder<> encoder;
  hpack::encoder<> decoder;
  // odd for client, even for server
  http2::stream_id_t stream_id;
  uint32_t refcount = 0;
  uint32_t my_window_size = http2::initial_window_size_for_connection_overall;
  uint32_t receiver_window_size = http2::initial_window_size_for_connection_overall;
  // setted only when writer is suspended and nullptr when works
  dd::job writer;
  requests_t requests;
  // 128 must be enough, TG max concurrent stream count is 100
  // Note: must be before 'responses' because of destroy ordering
  responses_t::bucket_type buckets[128];
  responses_t responses;
  timers_t timers;
  bool dropped = false;  // setted ONLY in drop_connection

  bi::slist<request_node, requests_member_hook, bi::constant_time_size<true>> free_nodes;

  explicit http2_connection(tcp_connection&& c) : asio_con(std::move(c)), responses({buckets, 128}) {
  }

  http2_connection(http2_connection&&) = delete;
  void operator=(http2_connection&&) = delete;

  ~http2_connection() {
    free_nodes.clear_and_dispose([](request_node* node) { delete node; });
  }

  [[nodiscard]] bool is_dropped() const noexcept {
    return dropped;
  }
  void start_drop() noexcept {
    assert(!dropped);
    dropped = true;
  }

  // interface for writer only

  struct work_waiter {
    http2_connection* connection = nullptr;
    TGBM_PIN;

    bool await_ready() const noexcept {
      return !connection->requests.empty() || connection->is_dropped();
    }
    void await_suspend(std::coroutine_handle<dd::job_promise> writer) noexcept {
      assert(connection->writer.handle == nullptr);
      connection->writer.handle = writer;
    }
    [[nodiscard]] bool await_resume() const noexcept {
      connection->writer.handle = nullptr;
      return !connection->is_dropped();
    }
  };

  // postcondition: if returned true, then !requests.empty() && connection not dropped
  // Note: worker may be still has no right to work (too many streams)
  [[nodiscard]] work_waiter wait_work() noexcept {
    return work_waiter(this);
  }

  [[nodiscard]] auto idle_yield() noexcept KELCORO_LIFETIMEBOUND {
    return dd::this_coro::suspend_and(
        [s = &socket()](std::coroutine_handle<> me) -> void { asio::post(s->get_executor(), me); });
  }
  [[nodiscard]] size_t concurrent_streams_now() noexcept {
    return responses.size();
  }

  // interface for reader

  [[nodiscard]] bool is_outof_streamids() const noexcept {
    return stream_id > http2::max_stream_id;
  }
  // TODO test выставляя максимум небольшим
  // when streamid is max, connection is not broken, but required to stop
  [[nodiscard]] bool is_done_completely() const noexcept {
    return is_outof_streamids() && requests.empty() && responses.empty();
  }

  void forget(request_node& node) noexcept {
    if (node.requests_hook.is_linked())
      erase_byref(requests, node);
    if (node.responses_hook.is_linked()) {
      assert(responses.count(node.req.streamid) == 1);
      erase_byref(responses, node);
    }
    if (node.timers_hook.is_linked())
      erase_byref(timers, node);
  }

  // precondition: node is in 'responses'
  // ALL requests must be finished by this call (except canceling by destroying node)
  void finish_request(request_node& node, int status) noexcept {
    forget(node);
    if (!node.task)
      return;
    LOG_DEBUG("[HTTP2]: stream {} finished, status: {}", node.req.streamid, status);
    node.status = status;
    auto t = std::exchange(node.task, nullptr);
    if (status == reqerr_e::cancelled)
      // ignore possible bad alloc for coroutine
      send_rst_stream(this, node.req.streamid, http2::errc_e::CANCEL).start_and_detach();
    t.resume();
  }

  // returns false is no such stream
  [[nodiscard]] bool finish_stream_with_error(http2::rst_stream rst_frame) noexcept {
    auto* node = find_response_by_streamid(rst_frame.header.stream_id);
    if (!node)
      return false;
    finish_request(*node, reqerr_e::server_cancelled_request);
    return true;
  }

  void finish_request_by_timeout(request_node& node) noexcept {
    finish_request(node, reqerr_e::timeout);
  }

  void finish_all_with_exception(reqerr_e::values reason) {
    assert(is_dropped());  // must be called only while drop_connection()

    // assume only i have access to it
    auto reqs = std::move(requests);
    auto rsps = std::move(responses);
    assert(reqs.size() + rsps.size() == timers.size());
    // nodes in reqs or in rsps, timers do not own them
    timers.clear();
    LOG_DEBUG("finish {} requests and {} responses, reason code: {}", reqs.size(), rsps.size(), (int)reason);
    auto forget_and_resume = [&](request_node* node) { finish_request(*node, reason); };
    reqs.clear_and_dispose(forget_and_resume);
    rsps.clear_and_dispose(forget_and_resume);

    assert(requests.empty() && responses.empty() && "someone push request while connection finishes!");
  }

  [[nodiscard]] request_node* find_response_by_streamid(http2::stream_id_t id) noexcept {
    auto it = responses.find(id);
    if (it == responses.end())
      return nullptr;
    return &*it;
  }

  void drop_timeouted() {
    // prevent destruction of *this while resuming
    http2_connection_ptr lock = this;
    while (!timers.empty() && timers.top()->deadline.is_reached())
      // node deleted from timers by forgetting
      finish_request_by_timeout(*timers.top());
  }

  void window_update(http2::window_update_frame frame) {
    if (frame.header.stream_id == 0) {
      http2::increment_window_size(receiver_window_size, frame.window_size_increment);
      return;
    }
    request_node* node = find_response_by_streamid(frame.header.stream_id);
    if (!node)
      return;
    http2::increment_window_size(node->streamlevel_window_size, frame.window_size_increment);
  }

  asio::ssl::stream<asio::ip::tcp::socket>& socket() noexcept {
    return asio_con.socket;
  }

  // terminates on exception (must not throw)
  void shutdown(reqerr_e::values reason) noexcept {
    // prevents me to be destroyed while resuming writer/reader etc
    http2_connection_ptr lock = this;

    LOG_DEBUG("[HTTP2] [shutdown] connection, address: {}", (void*)this);
    if (is_dropped())
      return;
    // set flag for anyone who will be resumed while shutting down this connection
    start_drop();
    // firstly stop handling new data on connection
    if (writer.handle) {
      writer.handle.destroy();
      writer.handle = nullptr;
    } else {
      // writer not waits for work, but suspended (because we are in single thread and working now)
      // == its in asio
      // then writer must be canceled by socket.cancel() or shutdown
    }
    finish_all_with_exception(reason);
    asio_con.shutdown();
  }

  // interface for send_request

  // postcondition: returns correct streamid (<= max_stream_id)
  [[nodiscard]] http2::stream_id_t next_streamid() noexcept {
    assert(stream_id <= http2::max_stream_id);
    assert((stream_id % 2) == 1);  // client side
    auto id = stream_id;
    stream_id += 2;
    return id;
  }

  node_ptr new_request_node(http_request&& request, std::string_view host, deadline_t deadline,
                            on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part) {
    node_ptr node = nullptr;
    if (free_nodes.empty())
      // workaround gcc12 bug by initializing union member explicitly
      node = new request_node{.writer_handle = nullptr};
    else {
      node = &free_nodes.front();
      free_nodes.pop_front();
    }
    node->streamlevel_window_size = server_settings.initial_stream_window_size;
    node->req = form_http2_request(std::move(request), next_streamid(), encoder, host);
    node->deadline = deadline;
    node->writer_handle = std::coroutine_handle<>(writer.handle);
    node->connection = this;
    node->on_header = on_header;
    node->on_data_part = on_data_part;
    node->status = reqerr_e::unknown_err;
    assert(node->refcount == 1);
    assert(!node->requests_hook.is_linked());
    assert(!node->responses_hook.is_linked());
    assert(!node->timers_hook.is_linked());
    return node;
  }

  void return_node(request_node* ptr) noexcept {
    assert(ptr);
    forget(*ptr);
    if (free_nodes.size() >= std::min<size_t>(1024, server_settings.max_concurrent_streams)) {
      delete ptr;
      return;
    }
    free_nodes.push_front(*ptr);
  }

  // returned value must be co_awaited
  // await resume returns request status
  [[nodiscard]] request_node& request_finished(request_node& node) noexcept {
    assert(!node.timers_hook.is_linked());
    assert(!node.requests_hook.is_linked());
    assert(!node.responses_hook.is_linked());
    requests.push_back(node);
    // highly likely, that new value will be at end,
    // because new deadline will be greater then previous
    if (node.deadline != deadline_t::never())
      timers.insert(timers.end(), node);
    return node;
  }
};

void intrusive_ptr_add_ref(http2_connection* p) {
  ++p->refcount;
}

void intrusive_ptr_release(http2_connection* p) noexcept {
  --p->refcount;
  if (p->refcount == 0)
    delete p;
}

void intrusive_ptr_add_ref(request_node* p) {
  ++p->refcount;
}

void intrusive_ptr_release(request_node* p) noexcept {
  --p->refcount;
  if (p->refcount == 0) {
    p->connection->return_node(p);
  }
}

// any finish request with cancelled
dd::task<void> send_rst_stream(http2_connection_ptr con, http2::stream_id_t streamid, http2::errc_e errc) {
  if (!con || con->is_dropped())
    co_return;
  byte_t bytes[http2::rst_stream::len];
  http2::rst_stream::form(streamid, errc, bytes);
  io_error_code ec;
  co_await net.write(con->socket(), bytes, ec);
  if (ec)
    LOG_DEBUG("HTTP/2: cannot rst stream, ec: {}", ec.what());
}

dd::task<bool> send_ping(http2_connection_ptr con, uint64_t data, bool request_pong) {
  if (!con || con->is_dropped())
    co_return false;
  io_error_code ec;
  byte_t buf[http2::ping_frame::len];
  http2::ping_frame::form(data, request_pong, buf);
  co_await net.write(con->socket(), buf, ec);
  co_return !ec;
}

struct first_settings_frame_visitor {
  http2::settings_t& settings;

  constexpr void operator()(http2::setting_t s) const {
    using namespace http2;
    switch (s.identifier) {
      case SETTINGS_HEADER_TABLE_SIZE:
        settings.header_table_size = s.value;
        return;
      case SETTINGS_ENABLE_PUSH:
        if (s.value > 0)
          throw protocol_error{};  // server MUST NOT send 1
        settings.enable_push = s.value;
        return;
      case SETTINGS_MAX_CONCURRENT_STREAMS:
        settings.max_concurrent_streams = s.value;
        return;
      case SETTINGS_INITIAL_WINDOW_SIZE:
        settings.initial_stream_window_size = s.value;
        return;
      case SETTINGS_MAX_FRAME_SIZE:
        settings.max_frame_size = s.value;
        return;
      case SETTINGS_MAX_HEADER_LIST_SIZE:
        settings.max_header_list_size = s.value;
        return;
      case SETTINGS_NO_RFC7540_PRIORITIES:
        if (s.value > 1)
          throw protocol_error{};
        settings.deprecated_priority_disabled = s.value;
      default:
          // ignore if dont know
          ;
    }
  };
};

static dd::task<void> handle_ping(http2::ping_frame ping, http2_connection_ptr con) {
  LOG_DEBUG("[HTTP2]: received ping, data: {}", ping.get_data());
  if (ping.header.flags & http2::flags::ACK)
    co_return;
  if (!co_await send_ping(std::move(con), ping.get_data(), false))
    LOG_ERR("[HTTP2]: cannot handle ping");
}

static dd::task<http2_connection_ptr> establish_http2_session(tcp_connection&& asio_con,
                                                              http2_client_options options) {
  using namespace http2;
  using enum http2::frame_e;

  http2_connection_ptr con = new http2_connection(std::move(asio_con));
  con->client_settings = http2::settings_t{
      .header_table_size = options.hpack_dyntab_size,
      .enable_push = false,
      // means nothing, since server do not start streams
      .max_concurrent_streams = settings_t::max_max_concurrent_streams,
      .initial_stream_window_size = http2::max_window_size,
      .max_frame_size = options.max_receive_frame_size,
      .deprecated_priority_disabled = true,
  };
  con->encoder = hpack::encoder<>(con->client_settings.header_table_size);
  con->stream_id = 1;  // client

  io_error_code ec;

  // send client connection preface

  bytes_t connection_request;

  form_connection_initiation(con->client_settings, std::back_inserter(connection_request));
  auto& socket = con->socket();
  size_t written = co_await net.write(socket, connection_request, ec);
  if (ec)
    throw network_exception("cannot write HTTP/2 client connection preface, err: {}", ec.what());

  // read server connection preface (settings frame)

  byte_t buf[h2fhl];
  (void)co_await net.read(socket, std::span(buf, h2fhl), ec);
  if (ec)
    throw network_exception("cannot read HTTP/2 server preface, {}", ec.what());
  frame_header header = frame_header::parse(buf);
  if (header.type != SETTINGS || header.length > frame_len_max)
    throw connection_error{};  // TODO send GOAWAY frame
  bytes_t bytes(header.length);
  (void)co_await net.read(socket, bytes, ec);
  if (ec)
    throw network_exception(ec);
  if (accepted_settings_frame() != header)
    settings_frame::parse(header, bytes, first_settings_frame_visitor(con->server_settings));
  // answer settings ACK as soon as possible

  accepted_settings_frame().send_to(buf);
  co_await net.write(socket, std::span(buf, h2fhl), ec);
  if (ec)
    throw network_exception("cannot send accepted settings frame to server, {}", ec.what());

  if (std::ranges::equal(buf, bytes)) {  // server ACK already received
    LOG("HTTP/2 connection successfully established without server settings frame");
    // con.decoder is default with 4096 bytes
    con->server_settings.max_frame_size =
        std::min(con->server_settings.max_frame_size, options.max_send_frame_size);
    co_return con;
  }
  // read server settings ACK and all other frames it sends now

  for (;;) {
    co_await net.read(socket, std::span(buf, h2fhl), ec);
    if (ec)
      throw network_exception(ec);
    header = frame_header::parse(buf);
    bytes.resize(header.length);
    co_await net.read(socket, bytes, ec);
    if (ec)
      throw network_exception(ec);
    switch (header.type) {
      case SETTINGS:
        if (header.flags & flags::ACK) {
          if (header.length != 0 || header.stream_id != 0)
            throw protocol_error{};
          con->decoder = hpack::encoder<>(con->server_settings.header_table_size);
          LOG("HTTP/2 connection successfully established, decoder size: {}",
              con->server_settings.header_table_size);
          con->server_settings.max_frame_size =
              std::min(con->server_settings.max_frame_size, options.max_send_frame_size);
          co_return con;
        }
        settings_frame::parse(header, bytes, server_settings_visitor(con->server_settings));
        continue;
      case WINDOW_UPDATE:
        con->window_update(window_update_frame::parse(header, bytes));
        continue;
      case PING:
        co_await handle_ping(ping_frame::parse(header, bytes), con);
        continue;
      case GOAWAY:
        goaway_frame::parse_and_throw_goaway(header, bytes);
      case PRIORITY:
      case PRIORITY_UPDATE:
        // ignore
        continue;
      case PUSH_PROMISE:
      case RST_STREAM:
      case DATA:
      case HEADERS:
      case CONTINUATION:
        // server MUST NOT initiate streams
        throw protocol_error{};
      default:
        // ignore extensions
        continue;
    }
  }
  unreachable();
}

void http2_client::notify_connection_waiters(http2_connection_ptr result) noexcept {
  // assume only i have access to waiters
  auto waiters = std::move(connection_waiters);
  LOG_DEBUG("[HTTP2]: resuming connection waiters, count: {}, connection: {}", waiters.size(),
            (void*)result.get());
  assert(connection_waiters.empty());  // check boost really works as expected
  waiters.clear_and_dispose([&](noexport::waiter_of_connection* w) {
    assert(!w->result);
    w->result = result;
    assert(w->task);
    // ordering matters for getting correct stream id
    w->task.resume();
  });
}

dd::job start_pinger_for(http2_connection_ptr con, duration_t interval) {
  if (!con || con->is_dropped())
    co_return;
  LOG_DEBUG("[HTTP2]: pinger started for {}", (void*)con.get());
  on_scope_exit {
    LOG_DEBUG("[HTTP2]: pinger for {} completed", (void*)con.get());
  };
  asio::steady_timer timer(con->socket().get_executor());
  io_error_code ec;
  http2::stream_id_t lastid = con->stream_id;
  for (;;) {
    co_await net.sleep(timer, interval, ec);
    if (con->is_dropped() || ec)
      co_return;
    if (lastid == con->stream_id)
      (void)co_await send_ping(con, 0, false);
    else
      lastid = con->stream_id;
    if (con->is_dropped())
      co_return;
  }
}

dd::job http2_client::start_connecting() {
  assert(!connection);
  co_await std::suspend_always{};  // resumed when needed by creator
  if (stop_requested) {
    LOG_DEBUG("[HTTP2] connection tries to create when stop requested, ignored");
    co_return;
  }
  if (already_connecting())
    co_return;

  http2_connection_ptr new_connection = nullptr;

  try {
    // note: connection unlocked before notify, avoiding this:
    // * first request drops connection,
    // * starting new connection
    // * ignore new connection (connections locked)
    // * no new requests, all stopped.
    {
      auto lock = lock_connections();
      on_scope_exit {
        lock.release();
        notify_connection_waiters(new_connection);
      };
      tcp_connection_ptr asio_con = co_await tcp_connection::create(
          io_ctx, std::string(get_host()), make_ssl_context_for_http2(), tcp_options);
      new_connection = co_await establish_http2_session(std::move(*asio_con), options);
    }
    assert(!connection);
    assert(new_connection);
    connection = new_connection;
    start_reader_for(new_connection);
    connection->writer.handle = nullptr;
    // writer itself sets writer->handle
    start_writer_for(new_connection);
    if (options.ping_interval != duration_t::max())
      start_pinger_for(new_connection, options.ping_interval);
  } catch (std::exception& e) {
    LOG_ERR("exception while trying to connect: {}", e.what());
  }
}

static bool validate_frame_header(const http2::frame_header& h) noexcept {
  return h.length <= http2::max_header_length && h.stream_id <= http2::max_header_length;
}

static bool strip_padding(std::span<byte_t>& bytes) {
  if (bytes.empty())
    return false;
  int padlen = bytes[0];
  if (padlen + 1 > bytes.size())
    return false;
  remove_prefix(bytes, 1);
  remove_suffix(bytes, padlen);
  return true;
}

// 'out' must contain atleast 9 bytes
static http2::frame_header form_headers_header(const prepared_http_request& request) noexcept {
  using enum http2::frame_e;
  using namespace http2::flags;
  return http2::frame_header{
      .length = uint32_t(request.headers.size()),
      .type = HEADERS,
      .flags = static_cast<http2::flags_t>(request.data.empty() ? END_HEADERS | END_STREAM : END_HEADERS),
      .stream_id = request.streamid,
  };
}

[[nodiscard]] static http2::frame_header form_data_header(uint32_t streamid, uint32_t frame_sz,
                                                          bool is_last) noexcept {
  using enum http2::frame_e;
  using namespace http2::flags;

  return http2::frame_header{
      .length = frame_sz,
      .type = DATA,
      .flags = is_last ? END_STREAM : EMPTY_FLAGS,
      .stream_id = streamid,
  };
}

// precondition: node.req.data is not empty
// forms new data frame
// also handles window size changes
// returns length of result DATA frame
// or 0 if cannot send because of control flow
// precondition: 'out' contains atleast 9 valid bytes
[[nodiscard]] static size_t fill_data_header(const request_node& node, const http2_connection& con,
                                             size_t unhandled_bytes, byte_t* out) noexcept {
  using enum http2::frame_e;
  using namespace http2::flags;

  assert(!node.req.data.empty());
  assert(node.req.data.size() >= unhandled_bytes);
  http2::frame_header header;

  header.length = std::min<size_t>({unhandled_bytes, con.server_settings.max_frame_size,
                                    node.streamlevel_window_size, con.receiver_window_size});
  header.type = DATA;
  header.flags = unhandled_bytes == header.length ? END_STREAM : EMPTY_FLAGS;
  header.stream_id = node.req.streamid;
  header.send_to(out);

  return header.length;
}

dd::task<void> write_pending_data_frames(node_ptr work, size_t handled_bytes, http2_connection_ptr con) {
  assert(con && work);

  assert(handled_bytes < work->req.data.size());
  io_error_code ec;
  prepared_http_request& req = work->req;

  uint32_t frame_len = 0;
  int status = reqerr_e::cancelled;
  auto& streamid = req.streamid;
  on_scope_exit {
    if (work->ignores_response())
      con->finish_request(*work, status);
  };
  if (handled_bytes < h2fhl) {
    LOG_DEBUG("[HTTP2] required to relocate {} bytes. Handled {}", req.data.size(), handled_bytes);
    // code above reuses already sended bytes as buffer for frame_header
    req.data.insert(req.data.begin(), h2fhl - handled_bytes, 0);
    handled_bytes = h2fhl;
  }
  assert(handled_bytes >= h2fhl);
  for (byte_t* in = req.data.data() + handled_bytes, *data_end = req.data.data() + req.data.size();
       in != data_end; in += frame_len) {
    if (work->finished() || con->is_dropped())
      co_return;
    frame_len = fill_data_header(*work, *con, std::distance(in, data_end), in - h2fhl);
    if (frame_len == 0) {
      LOG_DEBUG("[HTTP2] cannot send bytes now! unhandled: {}, max_frame_len: {}, stream wsz {}, con wsz: {}",
                std::distance(in, data_end), con->server_settings.max_frame_size,
                work->streamlevel_window_size, con->receiver_window_size);
      co_await con->idle_yield();
      continue;
    }

    // send frame

    co_await net.write(con->socket(), std::span(in - h2fhl, frame_len + h2fhl), ec);

    if (ec) {
      if (ec != asio::error::operation_aborted) {
        status = reqerr_e::network_err;
        LOG_DEBUG("[HTTP2]: sending pending data frames ended with network err: {}", ec.what());
      }
      co_return;
    }
    // control flow
    con->receiver_window_size -= frame_len;
    work->streamlevel_window_size -= frame_len;
  }  // end loop
  LOG_DEBUG("[HTTP2]: pending data frames for stream {} successfully sended", streamid);
  status = reqerr_e::done;
  co_return;
}

// handles con.requests, fills buf with their headers and data,
// fills 'handled_ignorers' with requests which ignore responses and handled
// returns filled bytes
// if unfinished_bytes != -1, then first con.buf is unfinished and 'unfinished_bytes' contains how many
// handled precondition: con.requests not empty
// also handles control flow and sends utility frames window_update for streams/connection
[[nodiscard]] std::span<const byte_t> fill_buffer(http2_connection& con, reusable_buffer& buf,
                                                  size_t& unfinished_handled,
                                                  std::vector<node_ptr>& handled_ignorers,
                                                  size_t& data_sended) {
  assert(!con.requests.empty());
  request_node* node = &con.requests.front();
  http2_frame_buffer frames(buf.get_atleast(node->req.headers.size() + h2fhl));
  unfinished_handled = size_t(-1);
  data_sended = 0;

  goto start_filling_frames;
  for (;;) {
    // headers

    if (frames.available() < node->req.headers.size() + h2fhl)
      return frames.filled_bytes();
  start_filling_frames:
    if (!frames.push_frame(form_headers_header(node->req), node->req.headers))
      unreachable();

    // data

    if (node->req.data.empty())
      goto handle_next_node;
    // TODO control flow (calculate how many streams started)

    for (size_t handled_bytes = 0; handled_bytes != node->req.data.size();) {
      size_t unhandled_now = node->req.data.size() - handled_bytes;
      size_t frame_sz =
          std::min<size_t>({unhandled_now, frames.available_payload(), con.server_settings.max_frame_size,
                            node->streamlevel_window_size, con.receiver_window_size});
      if (frame_sz == 0) {
        unfinished_handled = handled_bytes;
        return frames.filled_bytes();
      }
      if (!frames.push_frame(form_data_header(node->req.streamid, frame_sz, frame_sz == unhandled_now),
                             node->req.data.data() + handled_bytes))
        unreachable();
      handled_bytes += frame_sz;
      // control flow
      data_sended += handled_bytes;
      node->streamlevel_window_size -= handled_bytes;
    }  // end loop filling data frames

  handle_next_node:
    con.requests.pop_front();
    if (!node->ignores_response())
      con.responses.insert(*node);
    else
      handled_ignorers.push_back(node);
    if (con.requests.empty())
      return frames.filled_bytes();
    node = &con.requests.front();
  }  // end loop filling buffer
}

// writes requests, handles control flow on sending side
// writer works on node with reader (window_update / rst_stream possible)
// also node may be cancelled or destroyed, so writer and reader must never cache node
// between co_awaits
// TODO вынести функцию записи в буфер отдельно, чтобы её можно было вызывать с разными запросами и стейтом
// коннекш
dd::job http2_client::start_writer_for(http2_connection_ptr con) {
  assert(con);
  LOG_DEBUG("[HTTP2]: writer started for {}", (void*)con.get());
  on_scope_exit {
    LOG_DEBUG("[HTTP2]: writer for {} completed", (void*)con.get());
  };

  io_error_code ec;
  reusable_buffer buffer;
  buffer.reserve(std::min<size_t>(con->server_settings.max_frame_size, 1024 * 16));
  for (;;) {
    // waiting for job or connection shutdown

    if (!co_await con->wait_work())
      goto end;
    assert(!con->requests.empty());

    // if != -1, then first node in request is unfinished (but its headers are sended)
    size_t unfinished_handled;
    std::vector<node_ptr> handled_ignorers;
    size_t data_sended;
    std::span bytes = fill_buffer(*con, buffer, unfinished_handled, handled_ignorers, data_sended);
    node_ptr unfinished = nullptr;
    if (unfinished_handled != -1) {
      assert(!con->requests.empty());
      unfinished = &con->requests.front();
      con->requests.pop_front();
      if (!unfinished->ignores_response())
        con->responses.insert(*unfinished);
    }

    // ignores con->concurrent_streams_now()
    // TODO if headers > con->settings.max_frame_size

    co_await net.write(con->socket(), bytes, ec);

    con->receiver_window_size -= data_sended;

    if (!handled_ignorers.empty()) {  // finish handled nodes if they ignore response
      int status = !ec                                    ? reqerr_e::done
                   : ec == asio::error::operation_aborted ? reqerr_e::cancelled
                                                          : reqerr_e::network_err;
      for (node_ptr& node : handled_ignorers)
        con->finish_request(*node, status);
      handled_ignorers.clear();
    }
    if (ec || con->is_dropped())
      goto end;
    if (unfinished && !unfinished->finished())
      write_pending_data_frames(std::move(unfinished), unfinished_handled, con).start_and_detach();
  }  // end loop handling requests
end:
  if (ec != asio::error::operation_aborted)
    LOG_DEBUG("[HTTP2] connection dropped with network err {}", ec.what());
  drop_connection(reqerr_e::network_err);
}

// handles only utility frames (not DATA / HEADERS), returns false on protocol error (may throw it too)
static bool handle_utility_frame(http2_frame_t&& frame, http2_connection& con) {
  using enum http2::frame_e;
  using namespace http2;

  switch (frame.header.type) {
    default:
    case HEADERS:
    case DATA:
      unreachable();
    case SETTINGS:
      /*
        note: ignores reducing dynamic table size and not sends encoder.dynamic size update after it.
        If it will break something its easily fixed by settings dyntab size to 0 initially
        and really not easy to implement this requirement
      */
      {
        auto before = con.server_settings.header_table_size;
        settings_frame::parse(frame.header, frame.data, server_settings_visitor(con.server_settings));
        if (before > con.server_settings.header_table_size)
          LOG("[HTTP2]: HPACK table resized, new size {}, before: {}, IF ERROR HAPPENS AFTER THIS MESSAGE, "
              "next time set dynamic table size for client to 0",
              con.server_settings.header_table_size, before);
      }
      return true;
    case PING:
      handle_ping(ping_frame::parse(frame.header, frame.data), &con).start_and_detach();
      return true;
    case RST_STREAM:
      if (!con.finish_stream_with_error(rst_stream::parse(frame.header, frame.data)))
        LOG_DEBUG("[HTTP2]: server finished stream (id: {}) which is not exists (maybe timeout or canceled)",
                  frame.header.stream_id);
      return true;
    case GOAWAY:
      goaway_frame::parse_and_throw_goaway(frame.header, frame.data);
    case WINDOW_UPDATE:
      con.window_update(window_update_frame::parse(frame.header, frame.data));
      return true;
    case PUSH_PROMISE:
      if (con.client_settings.enable_push)
        LOG_DEBUG("[HTTP2]: received PUSH_PROMISE, not supported");
      return false;
    case CONTINUATION:
      LOG_DEBUG("[HTTP2]: received CONTINUATION, not supported");
      return false;
    case PRIORITY_UPDATE:
    case PRIORITY:
      // ignore
      return true;
  }
}

// handles DATA or HEADERS, returns false on protocol error
[[nodiscard]] static bool handle_headers_or_data(http2_frame_t&& frame, http2_connection& con) noexcept {
  using enum http2::frame_e;

  assert(con.client_settings.deprecated_priority_disabled);
  if (frame.header.stream_id == 0)
    return false;
  if ((frame.header.flags & http2::flags::PADDED) && !strip_padding(frame.data))
    return false;
  if (frame.header.flags & http2::flags::PRIORITY) {
    LOG_DEBUG("[HTTP2]: received deprecated priority HEADERS, not supported");
    return false;
  }

  request_node* node = con.find_response_by_streamid(frame.header.stream_id);
  if (!node)
    return true;
  try {
    switch (frame.header.type) {
      case HEADERS:
        node->receive_headers(con.decoder, std::move(frame));
        if ((frame.header.flags & http2::flags::END_HEADERS) && !node->on_data_part)
          con.finish_request(*node, node->status);
        break;
      case DATA:
        // applicable only to data
        con.my_window_size -= frame.header.length;
        node->receive_data(std::move(frame), frame.header.flags & http2::flags::END_STREAM);
        break;
      default:
        unreachable();
    }
  } catch (protocol_error&) {
    return false;
  } catch (...) {
    // user-handling exception, do not drop connection
    node->task.promise().exception = std::current_exception();
    con.finish_request(*node, reqerr_e::user_exception);
    return true;
  }

  if (frame.header.flags & http2::flags::END_STREAM)
    con.finish_request(*node, node->status);
  return true;
}

// returns false on protocol error
[[nodiscard]] static bool handle_frame(http2_frame_t&& frame, http2_connection& con) {
  using enum http2::frame_e;
  switch (frame.header.type) {
    case HEADERS:
    case DATA:
      return handle_headers_or_data(std::move(frame), con);
    default:
      return handle_utility_frame(std::move(frame), con);
  }
}

dd::task<bool> send_window_update(http2_connection_ptr con, http2::stream_id_t id, uint32_t inc) {
  if (!con || con->is_dropped())
    co_return false;
  byte_t buf[http2::window_update_frame::len];
  http2::window_update_frame::form(id, inc, buf);
  LOG_DEBUG("[HTTP2]: update window, stream: {}, inc: {}, mywindowsiz: {}", id, inc, con->my_window_size);
  io_error_code ec;
  co_await net.write(con->socket(), std::span(buf), ec);
  co_return !ec;
}

// writer works on node with reader (window_update / rst_stream possible)
// also node may be cancelled or destroyed, so writer and reader must never cache node
// between co_awaits
dd::job http2_client::start_reader_for(http2_connection_ptr _con) {
  using enum http2::frame_e;
  using namespace http2;
  LOG_DEBUG("[HTTP2]: reader started for {}", (void*)_con.get());
  assert(_con);
  on_scope_exit {
    LOG_DEBUG("[HTTP2]: reader for {} ended", (void*)_con.get());
  };
  http2_connection& con = *_con;
  io_error_code ec;
  reusable_buffer buffer;
  http2_frame_t frame;
  int reason = reqerr_e::unknown_err;

  try {
    while (!con.is_done_completely()) {
      if (con.is_dropped())
        goto connection_dropped;

      // read header

      frame.data = buffer.get_exactly(h2fhl);

      co_await net.read(con.socket(), frame.data, ec);

      if (ec)
        goto network_error;
      if (con.is_dropped())
        goto connection_dropped;

      // parse header

      frame.header = frame_header::parse(std::span<const byte_t, h2fhl>(frame.data.data(), h2fhl));
      if (!validate_frame_header(frame.header))
        goto protocol_error;

      // read data

      frame.data = buffer.get_exactly(frame.header.length);
      co_await net.read(con.socket(), frame.data, ec);
      if (ec)
        goto network_error;
      if (con.is_dropped())
        goto connection_dropped;

      // handle data

      if (!handle_frame(std::move(frame), con))
        goto protocol_error;

      if (con.my_window_size < http2::max_window_size / 2) {
        uint32_t inc = http2::max_window_size - con.my_window_size;
        if (co_await send_window_update(_con, 0, inc))
          con.my_window_size += inc;
      }
    }
  } catch (protocol_error&) {
    LOG("[HTTP2]: protocol error happens");
    reason = reqerr_e::protocol_err;
    goto drop_connection;
  } catch (http2::goaway_frame_received& gae) {
    LOG("[HTTP2]: goaway received, info: {}, errc: {}", gae.debug_info, http2::errc2str(gae.error_code));
    reason = reqerr_e::server_cancelled_request;
    goto drop_connection;
  } catch (std::exception& se) {
    LOG("[HTTP2]: unexpected exception {}", se.what());
    reason = reqerr_e::unknown_err;
    goto drop_connection;
  } catch (...) {
    LOG("[HTTP2]: unknown exception happens");
    reason = reqerr_e::unknown_err;
    goto drop_connection;
  }

  assert(con.is_done_completely());
  // must not resume anyone with 'done', because no pending requests (completely done)
  reason = reqerr_e::done;
  goto drop_connection;
protocol_error:
  reason = reqerr_e::protocol_err;
  goto drop_connection;
network_error:
  reason = ec == asio::error::operation_aborted ? reqerr_e::cancelled : reqerr_e::network_err;
  if (reason == reqerr_e::network_err)
    LOG_DEBUG("[HTTP2]: reader drops connection after network err: {}", ec.what());
drop_connection:
  drop_connection(static_cast<reqerr_e::values>(reason));
connection_dropped:
  if (!connection_waiters.empty())
    co_await dd::this_coro::destroy_and_transfer_control_to(start_connecting().handle);
  co_return;
}

http2_client::~http2_client() {
  stop();
}

http2_client::http2_client(std::string_view host, http2_client_options opts, tcp_connection_options tcp_opts)
    : http_client(host), options(opts), io_ctx(1), tcp_options(tcp_opts) {
  options.max_receive_frame_size = std::min<size_t>(http2::frame_len_max, options.max_receive_frame_size);
  options.max_send_frame_size =
      // 8 KB now, because of strange TG behavior when sending big files with big frames, it do not respond
      std::min<size_t>(8 * 1024 /*http2::frame_len_max*/, options.max_send_frame_size);
}

noexport::waiter_of_connection::~waiter_of_connection() {
  // in case when .destroy on handle called correctly cancels request
  if (is_linked())
    erase_byref(client->connection_waiters, *this);
}

bool noexport::waiter_of_connection::await_ready() noexcept {
  assert(!client->stop_requested);
  if (!client->connection || client->connection->is_dropped() || client->connection->is_outof_streamids())
    return false;
  result = client->connection;
  return true;
}

std::coroutine_handle<> noexport::waiter_of_connection::await_suspend(std::coroutine_handle<> h) noexcept {
  task = h;
  client->connection_waiters.push_back(*this);
  if (client->already_connecting())
    return std::noop_coroutine();
  return client->start_connecting().handle;
}

[[nodiscard]] http2_connection_ptr noexport::waiter_of_connection::await_resume() const {
  if (!result || result->is_dropped() || client->stop_requested)
    return nullptr;
  return std::move(result);
}

// terminates on exception (because i cannot handle it)
void http2_client::drop_connection(reqerr_e::values reason) noexcept {
  http2_connection_ptr con = std::move(connection);
  if (!con)
    return;
  // note: i have shared ptr to con, so it will not be destroyed while shutting down
  // and resuming its reader/writer
  con->shutdown(reason);
}

dd::task<int> http2_client::send_request(on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part,
                                         http_request request, duration_t timeout) {
  assert((threadid == std::thread::id{} || threadid == std::this_thread::get_id()) &&
         "client must be used in one thread");
  // TODO retries (in the api)
  // TODO client-pool, client with retries(template), may be client with metrics (what recived/sended)
  // how much time request etc)
  if (stop_requested)
    co_return reqerr_e::cancelled;

  ++requests_in_progress;
  on_scope_exit {
    --requests_in_progress;
  };
  // calculates deadline before borrowing connection
  deadline_t deadline = deadline_after(timeout);
  http2_connection_ptr con = co_await borrow_connection();
  if (!con)
    co_return reqerr_e::network_err;

  node_ptr node = con->new_request_node(std::move(request), get_host(), deadline, on_header, on_data_part);

  LOG_DEBUG("send_request, path: {}, streamid: {}", request.path, node->req.streamid);
  co_return co_await con->request_finished(*node);
}

// runs until 'timer' is lockable && timer not cancelled,
// 'todo' must return duration_t (interval until next call)
dd::job periodic_task(std::weak_ptr<asio::steady_timer> timer, auto todo) {
  io_error_code ec;
  for (;;) {
    std::shared_ptr tmr = timer.lock();
    if (!tmr)
      co_return;
    duration_t interval = todo();
    co_await net.sleep(*tmr, interval, ec);
    if (ec) {
      if (ec != asio::error::operation_aborted)
        LOG_ERR("timer ended with error: {}", ec.what());
      co_return;
    }
  }
}

void http2_client::run() {
  TGBM_ON_DEBUG(threadid = std::this_thread::get_id(); on_scope_exit { threadid = std::thread::id{}; });
  stop_requested = false;
  on_scope_exit {
    stop_requested = false;
  };
  if (io_ctx.stopped())
    io_ctx.restart();
  std::shared_ptr<asio::steady_timer> timer = nullptr;
  if (options.timeout_check_interval != duration_t::max()) {
    timer = std::make_shared<asio::steady_timer>(io_ctx);
    periodic_task(timer, [client = this] {
      if (client->connection)
        client->connection->drop_timeouted();
      return client->options.timeout_check_interval;
    });
  }
  on_scope_exit {
    timer->cancel_one();
  };
  io_ctx.run();
}

bool http2_client::run_one(duration_t timeout) {
  stop_requested = false;
  on_scope_exit {
    stop_requested = false;
  };
  if (io_ctx.stopped())
    io_ctx.restart();
  auto count = io_ctx.run_one_for(timeout);
  if (connection)
    connection->drop_timeouted();
  return count > 0;
}

void http2_client::stop() {
  // avoids situations when stop() itself produces next stop()
  // for example in scope_exit on some coroutine which will be stopped while stopping
  if (stop_requested)
    return;
  stop_requested = true;
  drop_connection(reqerr_e::cancelled);
  while (requests_in_progress != 0)
    io_ctx.run_one();
  auto lock = lock_connections();
  notify_connection_waiters(nullptr);
  drop_connection(reqerr_e::cancelled);
  io_ctx.stop();
  lock.release();
  assert(!connection);
  assert(!is_connecting);
  assert(connection_waiters.empty());
  assert(requests_in_progress == 0);
}

}  // namespace tgbm
