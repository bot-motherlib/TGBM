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

#include <hpack/hpack.hpp>
#include <tgbm/net/http2/protocol.hpp>
#include <tgbm/net/http2/client.hpp>
#include <tgbm/logger.hpp>
#include <tgbm/utils/scope_exit.hpp>
#include <tgbm/utils/macro.hpp>
#include <tgbm/net/errors.hpp>
#include <tgbm/utils/reusable_buffer.hpp>
#include <tgbm/utils/deadline.hpp>

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/treap_set.hpp>
#include <boost/intrusive/list_hook.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/slist.hpp>

namespace tgbm {

namespace asio = boost::asio;

constexpr inline auto h2fhl = http2::frame_header_len;

struct http2_frame_t {
  http2::frame_header header;
  std::span<byte_t> data;
};

struct request_node;

using node_ptr = boost::intrusive_ptr<request_node>;

static void generate_http2_headers_to(const http_request& request, hpack::encoder& encoder,
                                      bytes_t& headers) {
  using hdrs = hpack::static_table_t::values;

  assert(!request.path.empty());
  assert(!(request.body.content_type.empty() && !request.body.data.empty()));

  auto out = std::back_inserter(headers);

  // required scheme, method, authority, path
  switch (request.scheme) {
    case scheme_e::HTTPS:
      encoder.encode_header_fully_indexed(hdrs::scheme_https, out);
      break;
    case scheme_e::HTTP:
      encoder.encode_header_fully_indexed(hdrs::scheme_http, out);
      break;
    default:
      unreachable();
  }
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
  encoder.encode<true>(hdrs::authority, request.authority, out);
  encoder.encode<true, true>(hdrs::path, request.path, out);
  encoder.encode_header_never_indexing(hdrs::user_agent, "kelbon", out);
  // content type and custom headers

  if (!request.body.data.empty())
    encoder.encode<true>(hdrs::content_type, request.body.content_type, out);
  for (auto& [name, value] : request.headers)
    encoder.encode<true>(name, value, out);
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
  http_request req;  // will be moved into writer
  http2::stream_id_t streamid = 0;
  deadline_t deadline;
  union {
    std::coroutine_handle<> writer_handle = {};  // setted initialy before 'await_suspend'
    dd::task<int>::handle_type task;             // setted by 'await_suspend' (requester)
  };
  http2_connection_ptr connection = nullptr;
  //  received resonse (filled by 'reader' in connection)
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

  // returns false on protocol errors
  // precondition: padding removed
  void receive_headers(hpack::decoder& decoder, http2_frame_t&& frame) {
    assert(frame.header.stream_id == streamid);
    assert(frame.header.type == http2::frame_e::HEADERS);
    // weird things like continuations, trailers, many header frames with CONTINUE etc not supported
    if (!(frame.header.flags & http2::flags::END_HEADERS))
      throw protocol_error{};
    const byte_t* in = frame.data.data();
    const byte_t* e = in + frame.data.size();
    status = decoder.decode_response_status(in, e);
    // headers must be decoded to maintain HPACK dynamic table in correct state
    hpack::decode_headers_block(decoder, std::span(in, e),
                                [&](std::string_view name, std::string_view value) {
                                  if (on_header)
                                    (*on_header)(name, value);
                                });
  }

  // returns false on protocol errors
  // precondition: padding removed
  void receive_data(http2_frame_t&& frame, bool end_stream) {
    assert(frame.header.stream_id == streamid);
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
      return v.streamid;
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
  any_connection tcp_con;
  hpack::encoder encoder;
  // https://www.rfc-editor.org/rfc/rfc9113.html#section-6.5.2-2.2.1
  // Server may send SETTINGS frame with reduced hpack table size,
  // this means request for client encoder to send dynamic_size_update
  //
  // if true, new value in server_settings.header_table_size
  bool encoder_table_size_change_requested = false;
  hpack::decoder decoder;
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

  explicit http2_connection(any_connection&& c) : tcp_con(std::move(c)), responses({buckets, 128}) {
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

  write_awaiter write(std::span<const byte_t> bytes, io_error_code& ec) {
    return write_awaiter{tcp_con, ec, bytes};
  }
  read_awaiter read(std::span<byte_t> buf, io_error_code& ec) {
    return read_awaiter{tcp_con, ec, buf};
  }

  [[nodiscard]] size_t concurrent_streams_now() noexcept {
    return responses.size();
  }

  // interface for reader

  [[nodiscard]] bool is_outof_streamids() const noexcept {
    return stream_id > http2::max_stream_id;
  }

  // when streamid is max, connection is not broken, but required to stop
  [[nodiscard]] bool is_done_completely() const noexcept {
    return is_outof_streamids() && requests.empty() && responses.empty();
  }

  void forget(request_node& node) noexcept {
    if (node.requests_hook.is_linked())
      erase_byref(requests, node);
    if (node.responses_hook.is_linked()) {
      assert(responses.count(node.streamid) == 1);
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
    TGBM_HTTP2_LOG(DEBUG, "stream {} finished, status: {}", node.streamid, status);
    node.status = status;
    auto t = std::exchange(node.task, nullptr);
    // != 0, so stream already started
    if (node.streamid != 0 && (status == reqerr_e::cancelled || status == reqerr_e::timeout))
      // ignore possible bad alloc for coroutine
      send_rst_stream(this, node.streamid, http2::errc_e::CANCEL).start_and_detach();
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
    // >= because request may not be inserted in 'timers' if deadline == never
    assert(reqs.size() + rsps.size() >= timers.size());
    // nodes in reqs or in rsps, timers do not own them
    timers.clear();
    TGBM_HTTP2_LOG(DEBUG, "finish {} requests and {} responses, reason code: {}", reqs.size(), rsps.size(),
                   (int)reason);
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
    while (!timers.empty() && timers.top()->deadline.is_reached()) {
      // node deleted from timers by forgetting
      finish_request_by_timeout(*timers.top());
    }
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

  // terminates on exception (must not throw)
  void shutdown(reqerr_e::values reason) noexcept {
    // prevents me to be destroyed while resuming writer/reader etc
    http2_connection_ptr lock = this;

    TGBM_HTTP2_LOG(DEBUG, "[shutdown] connection, address: {}", (void*)this);
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
    tcp_con.shutdown();
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

  node_ptr new_request_node(http_request&& request, deadline_t deadline, on_header_fn_ptr on_header,
                            on_data_part_fn_ptr on_data_part) {
    node_ptr node = nullptr;
    if (free_nodes.empty())
      // workaround gcc12 bug by initializing union member explicitly
      node = new request_node{.writer_handle = nullptr};
    else {
      node = &free_nodes.front();
      free_nodes.pop_front();
    }
    node->streamlevel_window_size = server_settings.initial_stream_window_size;
    node->req = std::move(request);
    node->streamid = 0;
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
    ptr->connection = nullptr;
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

  void ignore_frame(http2_frame_t frame) {
    switch (frame.header.type) {
      case http2::frame_e::HEADERS:
        // https://www.rfc-editor.org/rfc/rfc9113.html#section-6.8-19
        // maintain hpack dynamic table
        hpack::decode_headers_block(decoder, frame.data,
                                    [](std::string_view /*name*/, std::string_view /*value*/) {});
        return;
      case http2::frame_e::DATA:
        // NOTE: not using data.size(), since padding should be counted as received octets
        // ('data' does not contain padding)
        my_window_size -= frame.header.length;
        return;
      default:
        return;
    }
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
    if (p->connection)
      p->connection->return_node(p);
    else
      delete p;
  }
}

// any finish request with cancelled
dd::task<void> send_rst_stream(http2_connection_ptr con, http2::stream_id_t streamid, http2::errc_e errc) {
  if (!con || con->is_dropped())
    co_return;
  byte_t bytes[http2::rst_stream::len];
  http2::rst_stream::form(streamid, errc, bytes);
  io_error_code ec;
  (void)co_await con->write(bytes, ec);
  if (ec)
    TGBM_HTTP2_LOG(ERROR, "cannot rst stream, ec: {}", ec.what());
}

static dd::task<void> send_settings_ack(http2_connection_ptr con) {
  if (!con || con->is_dropped())
    co_return;
  bytes_t bytes;
  http2::accepted_settings_frame().send_to(std::back_inserter(bytes));
  io_error_code ec;
  (void)co_await con->write(bytes, ec);
  if (ec)
    TGBM_HTTP2_LOG(ERROR, "cannot send settings ACK, err: {}", ec.what());
}

static dd::task<bool> send_ping(http2_connection_ptr con, uint64_t data, bool request_pong) {
  if (!con || con->is_dropped())
    co_return false;
  io_error_code ec;
  byte_t buf[http2::ping_frame::len];
  http2::ping_frame::form(data, request_pong, buf);
  (void)co_await con->write(buf, ec);
  co_return !ec;
}

static dd::task<void> handle_ping(http2::ping_frame ping, http2_connection_ptr con) {
  TGBM_HTTP2_LOG(DEBUG, "received ping, data: {}", ping.get_data());
  if (ping.header.flags & http2::flags::ACK)
    co_return;
  if (!co_await send_ping(std::move(con), ping.get_data(), false))
    TGBM_HTTP2_LOG(DEBUG, "cannot handle ping");
}

static dd::task<http2_connection_ptr> establish_http2_session(any_connection&& tcp_con,
                                                              http2_client_options options) {
  using namespace http2;
  using enum http2::frame_e;

  http2_connection_ptr con = new http2_connection(std::move(tcp_con));
  con->client_settings = http2::settings_t{
      .header_table_size = options.hpack_dyntab_size,
      .enable_push = false,
      // means nothing, since server do not start streams
      .max_concurrent_streams = settings_t::max_max_concurrent_streams,
      .initial_stream_window_size = http2::max_window_size,
      .max_frame_size = options.max_receive_frame_size,
      .deprecated_priority_disabled = true,
  };
  con->decoder = hpack::decoder(con->client_settings.header_table_size);
  con->stream_id = 1;  // client

  io_error_code ec;

  // send client connection preface

  bytes_t connection_request;

  form_connection_initiation(con->client_settings, std::back_inserter(connection_request));
  size_t written = co_await con->write(connection_request, ec);
  if (ec)
    throw network_exception("cannot write HTTP/2 client connection preface, err: {}", ec.what());

  // read server connection preface (settings frame)

  byte_t buf[h2fhl];
  co_await con->read(std::span(buf, h2fhl), ec);
  if (ec)
    throw network_exception("cannot read HTTP/2 server preface, {}", ec.what());
  frame_header header = frame_header::parse(buf);
  if (header.type != SETTINGS || header.length > frame_len_max)
    throw connection_error{};
  bytes_t bytes(header.length);
  co_await con->read(bytes, ec);
  if (ec)
    throw network_exception(ec);
  if (accepted_settings_frame() != header)
    settings_frame::parse(header, bytes, server_settings_visitor(con->server_settings));
  // answer settings ACK as soon as possible

  accepted_settings_frame().send_to(buf);
  (void)co_await con->write(std::span(buf, h2fhl), ec);
  if (ec)
    throw network_exception("cannot send accepted settings frame to server, {}", ec.what());

  // TODO ? if ACK flag?
  if (std::ranges::equal(buf, bytes)) {  // server ACK already received
    TGBM_HTTP2_LOG(INFO, "connection successfully established without server settings frame");
    // con.encoder is default with 4096 bytes
    con->server_settings.max_frame_size =
        std::min(con->server_settings.max_frame_size, options.max_send_frame_size);
    co_return con;
  }

  // read server settings ACK and all other frames it sends now

  for (;;) {
    co_await con->read(std::span(buf, h2fhl), ec);
    if (ec)
      throw network_exception(ec);
    header = frame_header::parse(buf);
    bytes.resize(header.length);
    co_await con->read(bytes, ec);
    if (ec)
      throw network_exception(ec);
    switch (header.type) {
      case SETTINGS:
        if (header.flags & flags::ACK) {
          if (header.length != 0 || header.stream_id != 0)
            throw protocol_error{};
          con->encoder = hpack::encoder(con->server_settings.header_table_size);
          TGBM_HTTP2_LOG(INFO, "connection successfully established, decoder size: {}",
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
      case PUSH_PROMISE:
      case RST_STREAM:
      case DATA:
      case HEADERS:
      case CONTINUATION:
        // server MUST NOT initiate streams
        throw protocol_error{};
      default:
      case PRIORITY:
      case PRIORITY_UPDATE:
        // ignore
        continue;
    }
  }
  unreachable();
}

void http2_client::notify_connection_waiters(http2_connection_ptr result) noexcept {
  // assume only i have access to waiters
  auto waiters = std::move(connection_waiters);
  TGBM_HTTP2_LOG(DEBUG, "resuming connection waiters, count: {}, connection: {}", waiters.size(),
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

dd::job start_timeout_warden_for(http2_connection_ptr con, any_transport_factory& f, duration_t interval) {
  if (!con || con->is_dropped())
    co_return;
  TGBM_HTTP2_LOG(DEBUG, "timeout warden started for {}", (void*)con.get());
  on_scope_exit {
    TGBM_HTTP2_LOG(DEBUG, "timeout warden for {} completed", (void*)con.get());
  };
  io_error_code ec;
  for (;;) {
    co_await f.sleep(interval, ec);
    if (ec) {
      if (ec != asio::error::operation_aborted)
        TGBM_HTTP2_LOG(ERROR, "sleep error: {}", ec.what());
      co_return;
    }
    if (con->is_dropped())
      co_return;
    con->drop_timeouted();
  }
}

dd::job start_pinger_for(http2_connection_ptr con, any_transport_factory& f, duration_t interval) {
  if (!con || con->is_dropped())
    co_return;
  TGBM_HTTP2_LOG(DEBUG, "pinger started for {}", (void*)con.get());
  on_scope_exit {
    TGBM_HTTP2_LOG(DEBUG, "pinger for {} completed", (void*)con.get());
  };
  io_error_code ec;
  http2::stream_id_t lastid = con->stream_id;
  for (;;) {
    co_await f.sleep(interval, ec);
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

dd::job http2_client::start_connecting(deadline_t deadline) {
  co_await std::suspend_always{};  // resumed when needed by creator
  if (connection) {
    notify_connection_waiters(connection);
    co_return;
  }
  if (stop_requested) {
    TGBM_HTTP2_LOG(DEBUG, "connection tries to create when stop requested, ignored");
    notify_connection_waiters(nullptr);
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
      any_connection tcp_con = co_await factory.create_connection(get_host(), deadline);
      // Note: connection deadline not propagated here (TODO)
      new_connection = co_await establish_http2_session(std::move(tcp_con), options);
    }
    assert(!connection);
    assert(new_connection);
    connection = new_connection;
    start_reader_for(new_connection);
    connection->writer.handle = nullptr;
    // writer itself sets writer->handle
    start_writer_for(new_connection);
    if (options.ping_interval != duration_t::max())
      start_pinger_for(new_connection, factory, options.ping_interval);
    if (options.timeout_check_interval != duration_t::max())
      start_timeout_warden_for(new_connection, factory, options.timeout_check_interval);
  } catch (std::exception& e) {
    TGBM_HTTP2_LOG(ERROR, "exception while trying to connect: {}", e.what());
  }
}

static bool validate_frame_header(const http2::frame_header& h) noexcept {
  return h.length <= http2::frame_len_max && h.stream_id <= http2::max_stream_id;
}

[[nodiscard]] static http2::frame_header form_data_header(http2::stream_id_t streamid, uint32_t frame_sz,
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

  assert(!node.req.body.data.empty());
  assert(node.req.body.data.size() >= unhandled_bytes);
  http2::frame_header header;

  header.length = std::min<size_t>({unhandled_bytes, con.server_settings.max_frame_size,
                                    node.streamlevel_window_size, con.receiver_window_size});
  header.type = DATA;
  header.flags = unhandled_bytes == header.length ? END_STREAM : EMPTY_FLAGS;
  header.stream_id = node.streamid;
  header.send_to(out);

  return header.length;
}

static dd::task<void> write_pending_data_frames(node_ptr work, size_t handled_bytes, http2_connection_ptr con,
                                                any_transport_factory& factory, http2_client& client) {
  assert(con && work);

  assert(handled_bytes < work->req.body.data.size());
  io_error_code ec;
  bytes_t& data = work->req.body.data;

  uint32_t frame_len = 0;

  // TODO optimize since always used with 0 (write first 9 + 9 bytes frame) (not ignore control flow)
  if (handled_bytes < h2fhl) {
    TGBM_HTTP2_LOG(DEBUG, "required to relocate {} bytes. Handled {}", data.size(), handled_bytes);
    // code above reuses already sended bytes as buffer for frame_header
    data.insert(data.begin(), h2fhl - handled_bytes, 0);
    handled_bytes = h2fhl;
  }
  assert(handled_bytes >= h2fhl);
  for (byte_t* in = data.data() + handled_bytes, *data_end = data.data() + data.size(); in != data_end;
       in += frame_len) {
    if (work->finished() || con->is_dropped())
      co_return;
    frame_len = fill_data_header(*work, *con, std::distance(in, data_end), in - h2fhl);
    if (frame_len == 0) {
      TGBM_HTTP2_LOG(DEBUG,
                     "cannot send bytes now! unhandled: {}, max_frame_len: {}, stream wsz {}, con wsz: {}",
                     std::distance(in, data_end), con->server_settings.max_frame_size,
                     work->streamlevel_window_size, con->receiver_window_size);
      co_await factory.sleep(std::chrono::nanoseconds(500), ec);
      if (ec) {
        TGBM_HTTP2_LOG(ERROR, "something went wrong while sleeping", ec.what());
        if (ec == asio::error::operation_aborted)
          co_return;
        // continue, ignore sleep errors
      }
      continue;
    }

    // send frame

    (void)co_await con->write(std::span(in - h2fhl, frame_len + h2fhl), ec);

    if (ec) {
      if (ec != asio::error::operation_aborted) {
        TGBM_HTTP2_LOG(DEBUG, "sending pending data frames ended with network err: {}", ec.what());
        con->finish_request(*work, reqerr_e::network_err);
        client.drop_connection(reqerr_e::network_err);
      }
      co_return;
    }
    // control flow
    con->receiver_window_size -= frame_len;
    work->streamlevel_window_size -= frame_len;
  }  // end loop
  TGBM_HTTP2_LOG(DEBUG, "pending data frames for stream {} successfully sended", work->streamid);
  co_return;
}

// writes requests, handles control flow on sending side
// writer works on node with reader (window_update / rst_stream possible)
// also node may be cancelled or destroyed, so writer and reader must never cache node
// between co_awaits
dd::job http2_client::start_writer_for(http2_connection_ptr con) {
  assert(con);
  TGBM_HTTP2_LOG(DEBUG, "writer started for {}", (void*)con.get());
  on_scope_exit {
    TGBM_HTTP2_LOG(DEBUG, "writer for {} completed", (void*)con.get());
  };

  io_error_code ec;

  for (;;) {
    // waiting for job or connection shutdown

    if (!co_await con->wait_work())
      goto end;

    assert(!con->requests.empty());

    while (!con->requests.empty()) {
      node_ptr node = &con->requests.front();
      node->streamid = con->next_streamid();
      con->requests.pop_front();
      con->responses.insert(*node);

      // send headers

      // ignores con->concurrent_streams_now()
      // TODO if headers > con->settings.max_frame_size
      bytes_t headers(h2fhl, 0);  // reserve for frame header

      // https://www.rfc-editor.org/rfc/rfc9113.html#name-settings-synchronization
      if (con->encoder_table_size_change_requested) [[unlikely]] {
        con->encoder_table_size_change_requested = false;
        if (con->server_settings.header_table_size < con->encoder.dyntab.max_size()) {
          con->encoder.encode_dynamic_table_size_update(con->server_settings.header_table_size,
                                                        std::back_inserter(headers));
          con->encoder.dyntab.update_size(con->server_settings.header_table_size);
        }
        send_settings_ack(con).start_and_detach();
      }
      generate_http2_headers_to(node->req, con->encoder, headers);
      using namespace http2::flags;
      http2::frame_header headers_frame_header{
          .length = uint32_t(headers.size() - h2fhl),
          .type = http2::frame_e::HEADERS,
          .flags = http2::flags_t(node->req.body.data.empty() ? (END_HEADERS | END_STREAM) : END_HEADERS),
          .stream_id = node->streamid,
      };
      headers_frame_header.send_to(headers.data());

      (void)co_await con->write(headers, ec);

      if (ec || con->is_dropped()) {
        // otherwise will be finished by drop_connection with reqerr_e::cancelled
        if (ec != boost::asio::error::operation_aborted)
          con->finish_request(*node, reqerr_e::network_err);
        goto end;
      }
      // send data
      if (!node->req.body.data.empty())
        write_pending_data_frames(std::move(node), 0, con, factory, *this).start_and_detach();
    }
  }  // end loop handling requests
end:
  drop_connection(reqerr_e::network_err);
}

// handles only utility frames (not DATA / HEADERS), returns false on protocol error (may throw it too)
static bool handle_utility_frame(http2_frame_t&& frame, http2_connection& con) {
  using enum http2::frame_e;
  using namespace http2;

  switch (frame.header.type) {
    case HEADERS:
    case DATA:
      unreachable();
    case SETTINGS: {
      auto before = con.server_settings.header_table_size;
      server_settings_visitor vtor(con.server_settings);
      settings_frame::parse(frame.header, frame.data, vtor);
      if (before != con.server_settings.header_table_size) {
        TGBM_HTTP2_LOG(INFO, "HPACK table resized, new size {}, old size: {}",
                       con.server_settings.header_table_size, before);
        con.encoder_table_size_change_requested = true;
        // checking since start_writer_for sends ACK based on this information
        if (!(frame.header.flags & flags::ACK)) {
          TGBM_HTTP2_LOG(ERROR, "settings ACK, but size not zero");
          return false;
        }
      }
    }
      return true;
    case PING:
      handle_ping(ping_frame::parse(frame.header, frame.data), &con).start_and_detach();
      return true;
    case RST_STREAM:
      if (!con.finish_stream_with_error(rst_stream::parse(frame.header, frame.data)))
        TGBM_HTTP2_LOG(DEBUG,
                       "server finished stream (id: {}) which is not exists (maybe timeout or canceled)",
                       frame.header.stream_id);
      return true;
    case GOAWAY:
      goaway_frame::parse_and_throw_goaway(frame.header, frame.data);
    case WINDOW_UPDATE:
      con.window_update(window_update_frame::parse(frame.header, frame.data));
      return true;
    case PUSH_PROMISE:
      if (con.client_settings.enable_push)
        TGBM_HTTP2_LOG(DEBUG, "received PUSH_PROMISE, not supported");
      return false;
    case CONTINUATION:
      TGBM_HTTP2_LOG(DEBUG, "received CONTINUATION, not supported");
      return false;
    default:
    case PRIORITY:
    case PRIORITY_UPDATE:
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
  if ((frame.header.flags & http2::flags::PADDED) && !http2::strip_padding(frame.data))
    return false;
  if (frame.header.flags & http2::flags::PRIORITY) {
    TGBM_HTTP2_LOG(DEBUG, "received deprecated priority HEADERS, not supported");
    return false;
  }

  request_node* node = con.find_response_by_streamid(frame.header.stream_id);
  if (!node) {
    con.ignore_frame(frame);
    return true;
  }
  try {
    switch (frame.header.type) {
      case HEADERS:
        if (frame.header.flags & http2::flags::PRIORITY) [[unlikely]] {
          // options to disable priority is extension for protocol, it may not be supported
          // so i can receive it
          // ignores
          //  [Exclusive (1)],
          //  [Stream Dependency (31)],
          //  [Weight (8)]
          static_assert(CHAR_BIT == 8);
          if (frame.data.size() < 5)
            throw protocol_error{};
          remove_prefix(frame.data, 5);
        }
        node->receive_headers(con.decoder, std::move(frame));
        if ((frame.header.flags & http2::flags::END_HEADERS) && !node->on_data_part)
          con.finish_request(*node, node->status);
        break;
      case DATA:
        // applicable only to data
        // Note: includes padding!
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
  TGBM_HTTP2_LOG(DEBUG, "update window, stream: {}, inc: {}, mywindowsiz: {}", id, inc, con->my_window_size);
  io_error_code ec;
  (void)co_await con->write(std::span(buf), ec);
  co_return !ec;
}

// writer works on node with reader (window_update / rst_stream possible)
// also node may be cancelled or destroyed, so writer and reader must never cache node
// between co_awaits
dd::job http2_client::start_reader_for(http2_connection_ptr _con) {
  using enum http2::frame_e;
  using namespace http2;
  TGBM_HTTP2_LOG(DEBUG, "reader started for {}", (void*)_con.get());
  assert(_con);
  on_scope_exit {
    TGBM_HTTP2_LOG(DEBUG, "reader for {} ended", (void*)_con.get());
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

      // read frame header

      frame.data = buffer.get_exactly(h2fhl);

      co_await con.read(frame.data, ec);

      if (ec)
        goto network_error;
      if (con.is_dropped())
        goto connection_dropped;

      // parse frame header

      frame.header = frame_header::parse(frame.data);
      if (!validate_frame_header(frame.header))
        goto protocol_error;

      // read frame data

      frame.data = buffer.get_exactly(frame.header.length);
      co_await con.read(frame.data, ec);
      if (ec)
        goto network_error;
      if (con.is_dropped())
        goto connection_dropped;

      // handle frame

      if (!handle_frame(std::move(frame), con))
        goto protocol_error;

      if (con.my_window_size < http2::max_window_size / 2) {
        uint32_t inc = http2::max_window_size - con.my_window_size;
        if (co_await send_window_update(_con, 0, inc))
          con.my_window_size += inc;
      }
    }
  } catch (protocol_error&) {
    TGBM_HTTP2_LOG(INFO, "protocol error happens");
    reason = reqerr_e::protocol_err;
    goto drop_connection;
  } catch (http2::goaway_exception& gae) {
    TGBM_HTTP2_LOG(ERROR, "goaway received, info: {}, errc: {}", gae.debug_info,
                   http2::e2str(gae.error_code));
    reason = reqerr_e::server_cancelled_request;
    goto drop_connection;
  } catch (std::exception& se) {
    TGBM_HTTP2_LOG(INFO, "unexpected exception {}", se.what());
    reason = reqerr_e::unknown_err;
    goto drop_connection;
  } catch (...) {
    TGBM_HTTP2_LOG(INFO, "unknown exception happens");
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
    TGBM_HTTP2_LOG(DEBUG, "reader drops connection after network err: {}", ec.what());
drop_connection:
  drop_connection(static_cast<reqerr_e::values>(reason));
connection_dropped:
  if (!connection_waiters.empty() && !is_connecting)
    co_await dd::this_coro::destroy_and_transfer_control_to(
        start_connecting(deadline_after(std::chrono::seconds(10))).handle);
  co_return;
}

http2_client::~http2_client() {
  stop();
  // in any case, even if client stopped now, some requests may wait
  notify_connection_waiters(nullptr);
  drop_connection(reqerr_e::cancelled);
}

http2_client::http2_client(std::string_view host, http2_client_options opts, any_transport_factory tf)
    : http_client(host), options(opts), factory(tf ? std::move(tf) : default_transport_factory()) {
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
  return client->start_connecting(deadline).handle;
}

[[nodiscard]] http2_connection_ptr noexport::waiter_of_connection::await_resume() {
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
                                         http_request request, deadline_t deadline) {
  assert((threadid == std::thread::id{} || threadid == std::this_thread::get_id()) &&
         "client must be used in one thread");
  if (stop_requested) [[unlikely]]
    co_return reqerr_e::cancelled;
  if (deadline.is_reached()) [[unlikely]]
    co_return reqerr_e::timeout;
  ++requests_in_progress;
  on_scope_exit {
    --requests_in_progress;
  };
  http2_connection_ptr con = co_await borrow_connection(deadline);
  if (deadline.is_reached())
    co_return reqerr_e::timeout;
  if (!con)
    co_return reqerr_e::network_err;
  if (request.authority.empty())  // default host
    request.authority = get_host();
  request.scheme = con->tcp_con.is_https() ? scheme_e::HTTPS : scheme_e::HTTP;

  TGBM_HTTP2_LOG(DEBUG, "send_request, path: {}", request.path);

  node_ptr node = con->new_request_node(std::move(request), deadline, on_header, on_data_part);

  co_return co_await con->request_finished(*node);
}

void http2_client::run() {
  TGBM_ON_DEBUG(threadid = std::this_thread::get_id(); on_scope_exit { threadid = std::thread::id{}; });
  stop_requested = false;
  on_scope_exit {
    stop_requested = false;
  };
  factory.run();
}

bool http2_client::run_one(duration_t timeout) {
  stop_requested = false;
  on_scope_exit {
    stop_requested = false;
  };
  auto count = factory.run_one(timeout);
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
    factory.run_one(std::chrono::nanoseconds(10));
  auto lock = lock_connections();
  notify_connection_waiters(nullptr);
  drop_connection(reqerr_e::cancelled);
  factory.stop();
  lock.release();
  assert(!connection);
  assert(!is_connecting);
  assert(connection_waiters.empty());
  assert(requests_in_progress == 0);
}

dd::task<void> http2_client::sleep(duration_t duration, io_error_code& ec) {
  return factory.sleep(duration, ec);
}

}  // namespace tgbm
