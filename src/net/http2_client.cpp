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

#include "tgbm/net/http2/hpack.hpp"
#include "tgbm/net/http2/protocol.hpp"
#include "tgbm/net/http2_client.hpp"
#include "tgbm/net/asio_awaiters.hpp"
#include "tgbm/logger.h"
#include "tgbm/tools/scope_exit.h"
#include "tgbm/tools/macro.hpp"
#include "tgbm/net/errors.hpp"
#include "tgbm/tools/algorithm.hpp"
#include "tgbm/tools/reusable_buffer.hpp"

#include <fmt/ranges.h>

#include <kelcoro/channel.hpp>

#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/treap_set.hpp>
#include <boost/intrusive/list_hook.hpp>
#include <boost/intrusive/list.hpp>

namespace tgbm {

struct http2_frame_t {
  http2::frame_header header;
  std::span<byte_t> data;
};

struct prepared_http_request {
  http2::stream_id_t streamid = 0;
  bytes_t headers;
  bytes_t data;

  [[nodiscard]] bool empty() const noexcept {
    return headers.empty() && data.empty();
  }
};

struct request_node;

struct writer_work {
  prepared_http_request request;
  request_node* node = nullptr;  // if not nullptr, then writer MUST finish it
};

// request starts in connection.requests, then goes into connection.responses
struct request_node {
  bi::list_member_hook<link_option> requests_hook;
  bi::unordered_set_member_hook<link_option> responses_hook;
  bi::bs_set_member_hook<link_option> timers_hook;
  // 'send request' fills req, deadline and writer_handle
  prepared_http_request req;
  std::chrono::time_point<std::chrono::steady_clock> deadline;
  union {
    std::coroutine_handle<> writer_handle;                    // setted initialy before 'await_suspend'
    std::coroutine_handle<dd::task<int>::promise_type> task;  // setted by 'await_suspend' (requester)
  };
  http2_connection* connection = nullptr;
  // received resonse (filled by 'reader' in connection)
  on_header_fn_ptr on_header;
  on_data_part_fn_ptr on_data_part;
  int status = reqerr_e::unknown_err;
  TGBM_PIN;

  static bool await_ready() noexcept {
    return false;
  }
  std::coroutine_handle<> await_suspend(std::coroutine_handle<dd::task<int>::promise_type> h) noexcept {
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
      return fnv_hash(s);
    }
  };
  struct compare_by_deadline {
    bool operator()(const request_node& l, const request_node& r) const noexcept {
      return l.deadline < r.deadline;  // less means higher priority
    }
  };
};

struct http2_connection : std::enable_shared_from_this<http2_connection> {
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
  asio_ssl_connection asio_con;
  hpack::encoder<> encoder;
  hpack::encoder<> decoder;
  // odd for client, even for server
  http2::stream_id_t stream_id;

  uint32_t my_window_size = http2::initial_window_size_for_connection_overall;
  // !now ignored!
  uint32_t receiver_window_size = http2::initial_window_size_for_connection_overall;
  // setted only when writer is suspended and nullptr when works
  dd::job writer;

  // stream ->(send_request) requests ->(writer) responses ->(reader) end stream

  requests_t requests;
  // 128 must be enough, TG max concurrent stream count is 100
  // Note: must be before 'responses' because of destroy ordering
  responses_t::bucket_type buckets[128];
  responses_t responses;
  timers_t timers;

  bool dropped = false;  // setted ONLY in drop_connection

  TGBM_PIN;

  explicit http2_connection(asio_ssl_connection&& c) : asio_con(std::move(c)), responses({buckets, 128}) {
  }

  http2_connection(http2_connection&&) = delete;
  void operator=(http2_connection&&) = delete;

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
    [[nodiscard]] writer_work await_resume() const noexcept {
      connection->writer.handle = nullptr;
      if (connection->is_dropped())
        return {};
      assert(!connection->requests.empty());
      request_node& node = connection->requests.front();

      connection->requests.pop_front();
      assert(connection->responses.count(node.req.streamid) == 0);
      assert(!node.responses_hook.is_linked());
      writer_work work{std::move(node.req)};
      if (!node.on_header && !node.on_data_part)
        work.node = &node;  // node goes into writer, he will finish it
      else
        connection->responses.insert(node);
      return work;
    }
  };

  // postcondition: if returned true, then !requests.empty() && connection not dropped
  // Note: worker may be still has no right to work (too many streams)
  [[nodiscard]] work_waiter wait_work() noexcept {
    return work_waiter(this);
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
    // invariant: node not in requests and responses
    // at one time
    if (node.requests_hook.is_linked())
      erase_byref(requests, node);
    else if (node.responses_hook.is_linked()) {
      assert(node.req.empty());
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
    node.status = status;
    node.task.resume();
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
    LOG_DEBUG("stream {} timed out", node.req.streamid);
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

  void drop_timeouted(std::chrono::steady_clock::time_point until) {
    // avoid high cost of shared_ptr copy when no work to do
    if (timers.empty() || timers.top()->deadline >= until)
      return;
    // prevent destruction of *this while resuming
    auto lock = shared_from_this();
    assert(std::is_sorted(timers.begin(), timers.end(), request_node::compare_by_deadline{}));
    assert(timers.empty() ||
           std::ranges::min_element(timers, std::ranges::less{}, &request_node::deadline)->deadline ==
               timers.top()->deadline);
    while (!timers.empty() && timers.top()->deadline < until)
      finish_request_by_timeout(*timers.top());
  }

  void window_update(http2::window_update_frame frame) {
    if (frame.header.stream_id != 0)
      return;
    // ignore stream-level control flow, its setted to max, must be enough
    if (frame.window_size_increment == 0)
      throw protocol_error{};
    // avoid overflow
    if (uint64_t(receiver_window_size) + uint64_t(frame.window_size_increment) >
        uint64_t(http2::max_window_size))
      throw protocol_error{};
    receiver_window_size += frame.window_size_increment;
  }

  asio::ssl::stream<asio::ip::tcp::socket>& socket() noexcept {
    return asio_con.socket;
  }

  // terminates on exception (must not throw)
  void shutdown(reqerr_e::values reason) noexcept {
    // Note: shared ptr prevents me to be destroyed while resuming writer/reader etc
    std::shared_ptr lock = shared_from_this();

    LOG_DEBUG("[HTTP2] [shutdown] connection, address: {}", (void*)this);
    if (is_dropped()) {
      LOG_DEBUG("[HTTP2] connection {} double dropped, ignore", (void*)this);
      return;
    }

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
  // returned value must be co_awaited
  // await resume returns request status
  [[nodiscard]] request_node& request_finished(request_node& node) noexcept {
    assert(!node.timers_hook.is_linked());
    assert(!node.requests_hook.is_linked());
    assert(!node.responses_hook.is_linked());
    requests.push_back(node);
    // highly likely, that new value will be at end,
    // because new deadline will be greater then previous
    timers.insert(timers.end(), node);
    return node;
  }
};

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
        settings.initial_window_size = s.value;
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

static dd::task<void> handle_ping(http2::ping_frame ping,
                                  asio::ssl::stream<asio::basic_stream_socket<asio::ip::tcp>>& socket) {
  LOG_DEBUG("received ping, data: {}", ping.get_data());
  if (ping.header.flags & http2::flags::ACK)
    co_return;
  // send back same data as received
  bytes_t bytes;
  http2::ping_frame::form(ping.get_data(), /*request_answer=*/false, std::back_inserter(bytes));
  io_error_code ec;
  co_await net.write(socket, bytes, ec);
  if (ec && ec != asio::error::operation_aborted)
    LOG_ERR("error while handling ping, ignore, continue execution, what: {}", ec.what());
}

static dd::task<std::shared_ptr<http2_connection>> establish_http2_session(asio_ssl_connection&& asio_con) {
  using namespace http2;
  using enum http2::frame_e;

  std::shared_ptr con(std::make_shared<http2_connection>(std::move(asio_con)));
  con->client_settings = http2::settings_t{
      .header_table_size = 4096,
      .enable_push = false,
      .max_concurrent_streams =
          settings_t::max_max_concurrent_streams,     // means nothing, since server do not start streams
      .initial_window_size = http2::max_window_size,  // max, do not want to track stream level flow controls
      .max_frame_size = frame_len_max,
      .deprecated_priority_disabled = true,
  };
  con->encoder = hpack::encoder<>(con->client_settings.header_table_size);
  con->my_window_size = con->client_settings.initial_window_size;
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

  byte_t buf[frame_header_len];
  (void)co_await net.read(socket, std::span(buf, frame_header_len), ec);
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
  co_await net.write(socket, std::span(buf, frame_header_len), ec);
  if (ec)
    throw network_exception("cannot send accepted settings frame to server, {}", ec.what());

  if (std::ranges::equal(buf, bytes)) {  // server ACK already received
    LOG("HTTP/2 connection successfully established without server settings frame");
    // con.decoder is default with 4096 bytes
    co_return con;
  }
  // read server settings ACK and all other frames it sends now

  for (;;) {
    co_await net.read(socket, std::span(buf, frame_header_len), ec);
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
          // TODO? if current size less then 4096, then first headers must be with dyntab update
          // and same for other changes of dynamic table size to less then maximum now
          LOG("HTTP/2 connection successfully established, decoder size: {}",
              con->server_settings.header_table_size);
          co_return con;
        }
        settings_frame::parse(header, bytes, server_settings_filler(con->server_settings));
        continue;
      case WINDOW_UPDATE:
        con->window_update(window_update_frame::parse(header, bytes));
        continue;
      case PING:
        co_await handle_ping(ping_frame::parse(header, bytes), con->socket());
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
  co_return con;
}

static asio::ssl::context make_ssl_context_for_http2() {
  namespace ssl = asio::ssl;
  ssl::context sslctx(ssl::context::tlsv13_client);
  sslctx.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 |
                     ssl::context::single_dh_use | SSL_OP_NO_COMPRESSION);
  sslctx.set_default_verify_paths();
  const unsigned char alpn_protos[] = {0x02, 'h', '2'};  // HTTP/2
  if (0 != SSL_CTX_set_alpn_protos(sslctx.native_handle(), alpn_protos, sizeof(alpn_protos)))
    throw network_exception{"ALPN ctx broken {}", ERR_error_string(ERR_get_error(), nullptr)};
  return sslctx;
}

void http2_client::notify_connection_waiters(std::shared_ptr<http2_connection> result) noexcept {
  // assume only i have access to waiters
  auto waiters = std::move(connection_waiters);
  assert(connection_waiters.empty());  // check boost really works as expected
  waiters.clear_and_dispose([&](noexport::waiter_of_connection* w) {
    assert(!w->result);
    w->result = result;
    assert(w->task);
    // ordering matters for getting correct stream id
    w->task.resume();
  });
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

  std::shared_ptr<http2_connection> new_connection = nullptr;

  try {
    // note: connection unlocked before notify, avoiding this:
    // * first request drops connection,
    // * starting new connection
    // * ignore new connection (connections locked)
    // * no new requests, all stopped.
    on_scope_exit {
      notify_connection_waiters(new_connection);
    };
    auto lock = lock_connections();
    std::shared_ptr asio_con =
        co_await asio_ssl_connection::create(io_ctx, std::string(get_host()), make_ssl_context_for_http2());
    new_connection = co_await establish_http2_session(std::move(*asio_con));
    assert(!connection);
    assert(new_connection);
    connection = new_connection;
    start_reader_for(new_connection);
    connection->writer.handle = nullptr;
    // writer itself sets writer->handle
    start_writer_for(new_connection);
    lock.release();
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
  uint8_t padlen = bytes[0];
  if (padlen + 1 > bytes.size())
    return false;
  // rm prefix 1, rm postfix padlen
  byte_t* b = bytes.data() + 1;
  byte_t* e = bytes.data() + (bytes.size() - padlen);
  bytes = std::span(b, e);
  return true;
}

dd::task<void> write_pending_data_frames(prepared_http_request request, size_t handled_bytes,
                                         std::shared_ptr<http2_connection> con,
                                         http2_connection::requests_t owned_node) {
  assert(con && !con->is_dropped());
  int status = reqerr_e::unknown_err;
  assert(owned_node.size() < 2);
  on_scope_exit {
    if (!owned_node.empty())
      con->finish_request(owned_node.front(), status);
  };
  http2::frame_header data_header{
      .length = 0,
      .type = http2::frame_e::DATA,
      .flags = http2::flags::EMPTY_FLAGS,
      .stream_id = request.streamid,
  };
  io_error_code ec;
  byte_t data_buf[http2::frame_header_len];
  size_t unhandled_bytes = request.data.size() - handled_bytes;
  for (;;) {
    // load max_frame_size from settings, because it may change between co_awaits
    const size_t max_frame_len = con->server_settings.max_frame_size;
    const bool last_frame = unhandled_bytes < max_frame_len;

    data_header.length = std::min(unhandled_bytes, max_frame_len);
    if (last_frame)
      data_header.flags = http2::flags::END_STREAM;
    data_header.send_to(data_buf);
    size_t handled_bytes = request.data.size() - unhandled_bytes;
    co_await net.write_many(con->socket(), ec, std::span(data_buf),
                            std::span(request.data).subspan(handled_bytes, data_header.length));
    if (ec) {
      if (ec != asio::error::operation_aborted)
        LOG_DEBUG("data frames for stream {} cannot be sended due network error, {}", request.streamid,
                  ec.what());
      status = reqerr_e::network_err;
      co_return;
    }
    if (con->is_dropped()) {
      LOG_DEBUG("data frames for stream {} cannot be sended due connection failure", request.streamid);
      status = reqerr_e::cancelled;
      co_return;
    }
    if (unhandled_bytes <= max_frame_len) {
      LOG_DEBUG("data frames for stream {} successfully sended", request.streamid);
      status = reqerr_e::done;
      co_return;
    }
    unhandled_bytes -= max_frame_len;
  }
  unreachable();
}

static void fill_headers_header(const prepared_http_request& request,
                                byte_t (&out)[http2::frame_header_len]) noexcept {
  using enum http2::frame_e;
  using namespace http2::flags;
  http2::frame_header header;
  header.length = uint32_t(request.headers.size());
  header.type = HEADERS;
  header.flags = request.data.empty() ? END_HEADERS | END_STREAM : END_HEADERS;
  header.stream_id = request.streamid;
  header.send_to(out);
};

// returns count of handled bytes from 'request.data'
[[nodiscard]] static size_t fill_data_header(prepared_http_request& request, size_t max_frame_size,
                                             byte_t (&out)[http2::frame_header_len]) noexcept {
  using enum http2::frame_e;
  using namespace http2::flags;
  assert(!request.data.empty());
  http2::frame_header header;
  header.length = std::min(request.data.size(), max_frame_size);
  header.type = DATA;
  header.flags = request.data.size() <= max_frame_size ? END_STREAM : EMPTY_FLAGS;
  header.stream_id = request.streamid;
  header.send_to(out);
  return header.length;
};

// writes requests, handles control flow on sending side
dd::job http2_client::start_writer_for(std::shared_ptr<http2_connection> con) {
  assert(con);
  io_error_code ec;
  on_scope_exit {
    LOG_DEBUG("writer for {} completed", (void*)con.get());
  };
  byte_t hdrs_buf[http2::frame_header_len];
  byte_t data_buf[http2::frame_header_len];

  // used for case when writer owns node and its .destroyed (unlinked from here)
  http2_connection::requests_t owned_node;
  for (;;) {
    auto [request, node] = co_await con->wait_work();
    if (request.empty())
      goto end;
    if (node) {
      assert(!node->requests_hook.is_linked());
      assert(owned_node.empty());
      owned_node.push_back(*node);
    }
    int status = reqerr_e::network_err;
    on_scope_exit {
      if (!owned_node.empty()) {
        request_node& n = owned_node.front();
        owned_node.pop_front();
        con->finish_request(n, status);
      }
    };
    while (con->concurrent_streams_now() >= con->server_settings.max_concurrent_streams) {
      LOG_DEBUG("many concurrent streams, writer is blocked, streams now: {}", con->concurrent_streams_now());
      co_await dd::this_coro::suspend_and([&](std::coroutine_handle<> h) {
        // set myself into end of task list while i cannot work
        asio::post(io_ctx, h);
      });
      if (con->is_dropped())
        goto end;
    }
    // TODO if headers > con->settings.max_frame_size
    fill_headers_header(request, hdrs_buf);
    if (!request.data.empty()) {
      size_t handled = fill_data_header(request, con->server_settings.max_frame_size, data_buf);
      co_await net.write_many(con->socket(), ec, std::span(hdrs_buf), std::span(request.headers),
                              std::span(data_buf), std::span(request.data.data(), handled));
      if (ec || con->is_dropped())
        goto end;
      if (handled < request.data.size())
        write_pending_data_frames(std::move(request), handled, con, std::move(owned_node)).start_and_detach();
    } else {  // data empty
      co_await net.write_many(con->socket(), ec, std::span(hdrs_buf), std::span(request.headers));
    }
    if (ec || con->is_dropped())
      goto end;

    status = reqerr_e::done;

    // add window_update frame if required
    if (con->my_window_size < http2::max_window_size / 2) [[unlikely]] {
      LOG_DEBUG("incrementing my window by {}", http2::max_window_size - con->my_window_size);
      byte_t inc_buf[http2::window_update_frame::len];
      http2::window_update_frame::form(0, http2::max_window_size - con->my_window_size, inc_buf);
      con->my_window_size = http2::max_window_size;
      co_await net.write(con->socket(), std::span(inc_buf), ec);
      if (ec || con->is_dropped())
        goto end;
    }
  }
end:
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
      settings_frame::parse(frame.header, frame.data, server_settings_filler(con.server_settings));
      return true;
    case PING:
      handle_ping(ping_frame::parse(frame.header, frame.data), con.socket()).start_and_detach();
      return true;
    case RST_STREAM:
      if (!con.finish_stream_with_error(rst_stream::parse(frame.header, frame.data)))
        LOG_DEBUG("server finished stream which is not exitsts (maybe timeout or canceled)");
      return true;
    case GOAWAY:
      goaway_frame::parse_and_throw_goaway(frame.header, frame.data);
    case WINDOW_UPDATE:
      con.window_update(window_update_frame::parse(frame.header, frame.data));
      return true;
    case PUSH_PROMISE:
      if (con.client_settings.enable_push)
        LOG_DEBUG("server send continuation, not supported");
      return false;
    case CONTINUATION:
      LOG_DEBUG("server send continuation, not supported");
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
    LOG_DEBUG("server sends deprecated priority HEADERS, unimplemented");
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
    node->status = reqerr_e::user_exception;
    con.finish_request(*node, node->status);
    return true;
  }

  if (frame.header.flags & http2::flags::END_STREAM)
    con.finish_request(*node, node->status);
  return true;
}

// returns false on protocol error
[[nodiscard]] static bool handle_frame(http2_frame_t&& frame, http2_connection& con) {
  using enum http2::frame_e;
  using namespace http2;
  switch (frame.header.type) {
    case HEADERS:
    case DATA:
      return handle_headers_or_data(std::move(frame), con);
    default:
      return handle_utility_frame(std::move(frame), con);
  }
}

dd::job http2_client::start_reader_for(std::shared_ptr<http2_connection> _con) {
  using enum http2::frame_e;
  using namespace http2;

  assert(_con);
  on_scope_exit {
    LOG_DEBUG("reader for {} ended", (void*)_con.get());
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

      frame.data = buffer.get_buffer(frame_header_len);
      co_await net.read(con.socket(), frame.data, ec);
      if (ec)
        goto network_error;
      if (con.is_dropped())
        goto connection_dropped;

      // parse header

      frame.header =
          frame_header::parse(std::span<const byte_t, frame_header_len>(frame.data.data(), frame_header_len));
      if (!validate_frame_header(frame.header))
        goto protocol_error;

      // read data

      frame.data = buffer.get_buffer(frame.header.length);
      co_await net.read(con.socket(), frame.data, ec);
      if (ec)
        goto network_error;
      if (con.is_dropped())
        goto connection_dropped;

      // handle data

      if (!handle_frame(std::move(frame), con))
        goto protocol_error;
    }
  } catch (protocol_error&) {
    reason = reqerr_e::protocol_err;
    goto drop_connection;
  } catch (http2::goaway_frame_received&) {
    reason = reqerr_e::server_cancelled_request;
    goto drop_connection;
  } catch (...) {
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

http2_client::http2_client(std::string_view host) : http_client(host), io_ctx(1) {
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

[[nodiscard]] std::shared_ptr<http2_connection> noexport::waiter_of_connection::await_resume() const {
  if (!result || result->is_dropped() || client->stop_requested)
    return nullptr;
  return std::move(result);
}

static bytes_t generate_http2_headers(const http_request& request, hpack::encoder<>& encoder,
                                      std::string_view host) {
  using hdrs = hpack::static_table_t::values;
  const bool get_req = request.body.data.empty();
  assert(!request.path.empty());
  assert(!(request.body.content_type.empty() && !request.body.data.empty()));
  /*
  TODO
  Once an endpoint acknowledges a change to SETTINGS_HEADER_TABLE_SIZE that reduces the maximum below the
  current size of the dynamic table, its HPACK encoder MUST start the next field block with a Dynamic Table
  Size Update instruction that sets the dynamic table to a size that is less than or equal to the reduced
  maximum
  */
  bytes_t headers;
  // after first request headers will be cached, so often its about 5 bytes
  headers.reserve(16);
  auto out = std::back_inserter(headers);

  // fill required scheme, method, authority, path

  encoder.encode_header_fully_indexed(hdrs::scheme_https, out);
  encoder.encode_header_fully_indexed(get_req ? hdrs::method_get : hdrs::method_post, out);
  encoder.encode_header_memory_effective<true, true>(hdrs::authority, host, out);
  encoder.encode_header_memory_effective<true, true>(hdrs::path, request.path, out);
  if (!get_req)
    encoder.encode_header_memory_effective<true>(hdrs::content_type, request.body.content_type, out);
  return headers;
}

static prepared_http_request form_http2_request(const http_request& request, http2::stream_id_t streamid,
                                                hpack::encoder<>& encoder, std::string_view host) {
  return prepared_http_request{
      .streamid = streamid,
      .headers = generate_http2_headers(request, encoder, host),
      .data = std::move(request.body.data),
  };
}

// terminates on exception (because i cannot handle it)
void http2_client::drop_connection(reqerr_e::values reason) noexcept {
  auto con = std::move(connection);
  if (!con)
    return;
  // note: i have shared ptr to con, so it will not be destroyed while shutting down
  // and resuming its reader/writer
  con->shutdown(reason);
}

dd::task<int> http2_client::send_request(on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part,
                                         http_request request, duration_t timeout) {
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
  std::chrono::time_point deadline = std::chrono::steady_clock::now() + timeout;
  std::shared_ptr con = co_await borrow_connection();
  if (!con)
    co_return reqerr_e::network_err;

  request_node node{
      .req = form_http2_request(request, con->next_streamid(), con->encoder, get_host()),
      .deadline = deadline,
      .writer_handle = std::coroutine_handle<>(con->writer.handle),
      .on_header = on_header,
      .on_data_part = on_data_part,
  };

  on_scope_exit {
    // erase node from all lists even on handle.destroy for correct request canceling
    con->forget(node);
  };
  int status = co_await con->request_finished(node);
  co_return status;
}

void http2_client::run() {
  stop_requested = false;
  on_scope_exit {
    stop_requested = false;
  };
  if (io_ctx.stopped())
    io_ctx.restart();
  std::shared_ptr timer = std::make_shared<asio::steady_timer>(io_ctx);
  struct timeout_warden {
    http2_client& client;
    std::weak_ptr<asio::steady_timer> timer;

    void operator()(const io_error_code& ec) noexcept {
      if (ec) {
        if (ec != asio::error::operation_aborted)
          LOG_ERR("timer ended with error: {}", ec.what());
        return;
      }
      std::shared_ptr tmr = timer.lock();
      if (!tmr) {
        LOG_DEBUG("timer down, task not");
        return;
      }
      if (client.connection)
        client.connection->drop_timeouted(std::chrono::steady_clock::now());
      tmr->expires_after(std::chrono::seconds(1));
      tmr->async_wait(std::move(*this));
    }
  };
  timer->expires_after(std::chrono::seconds(1));
  timer->async_wait(timeout_warden(*this, timer));
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
    connection->drop_timeouted(std::chrono::steady_clock::now());
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
