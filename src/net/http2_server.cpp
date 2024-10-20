/*
TODO docs about this file

*/

#include "tgbm/net/http2_server.hpp"

#include "tgbm/logger.h"
#include "tgbm/net/asio_awaiters.hpp"

#include "tgbm/net/ssl_context.hpp"
#include "tgbm/net/http2/protocol.hpp"
#include "tgbm/tools/memory.hpp"
#include "tgbm/tools/reusable_buffer.hpp"
#include "tgbm/tools/scope_exit.h"

#include <anyany/anyany.hpp>

namespace tgbm {

struct client_session;
using client_session_ptr = boost::intrusive_ptr<client_session>;

struct client_session_ctx {
  asio::strand<asio::io_context::executor_type> exe;
  asio::ssl::stream<asio::ip::tcp::socket> socket;
};

struct http2_stream_ctx {
  uint32_t streamlevel_window_size = 0;
};

dd::job start_handle_request(client_session_ptr session, http2::stream_id_t streamid, http_request req);

// holds all frames, writer writes them
struct frames_buffer {
  using on_sended_t = aa::any_with<aa::call<void()>, aa::move>;

  struct frame_to_send {
    http2::frame_header hdr;
    bytes_t data;
    // TODO rm ? или для стрим апдейта нужно будет ещё (стримы)
    on_sended_t on_sended;
  };
  // used to handle control flow and stopping
  client_session_ptr session;  // must not be null
  // TODO? сразу в один буфер складывать ? И просто отдельно список того что надо сделать после отправки
  bytes_t bytes_to_send;
  // TODO rm? std::vector<on_sended_t> todo_when_sended;
  std::coroutine_handle<> writer = nullptr;  // not nullptr if writer waits next frames

  explicit frames_buffer(client_session& ses) noexcept : session(&ses) {
  }

  // for writer
  KELCORO_CO_AWAIT_REQUIRED auto next_work();

  void push_response(http2::stream_id_t, http_response&& rsp);
  void push_ping_answer(http2::ping_frame);

 private:
  void push_headers(http2::stream_id_t streamid, std::span<const byte_t> hdrs, bool has_data);
  void push_data(http2::stream_id_t streamid, std::span<const byte_t> provider);

  // when all frames pushed, commit them (for not wake worker for each small part)
  // required only after 'push_frame'
  void commit() {
    if (writer) {
      std::coroutine_handle h = writer;
      writer = nullptr;
      h.resume();
    }
  }
  // dont forget 'commit'!
  void push_frame(http2::frame_header hdr, std::span<const byte_t> bytes) {
    byte_t b[http2::frame_header_len];
    hdr.send_to(b);
    bytes_to_send.insert(bytes_to_send.end(), std::begin(b), std::end(b));
    bytes_to_send.insert(bytes_to_send.end(), bytes.begin(), bytes.end());
  }
};

struct client_session {
  http2_server_ptr server;  // must not be null
  client_session_ctx session_ctx;
  http2::settings_t client_settings;
  http2::settings_t server_settings;
  // TODO intrusive all, опять request_node какой-то происходит
  std::unordered_map<http2::stream_id_t, http_request> incomplete_requests;
  std::unordered_map<http2::stream_id_t, http2_stream_ctx> incomplete_responses;
  http2::stream_id_t last_opened_stream = 0;
  uint32_t my_window_size = http2::initial_window_size_for_connection_overall;
  uint32_t receiver_window_size = http2::initial_window_size_for_connection_overall;
  hpack::decoder decoder;
  hpack::encoder encoder;
  frames_buffer framebuf;
  // for supporting SETTINGS table reduction while connection in progress
  uint32_t table_size_decrease = 0;
  bool shutted_down = false;

  size_t refcount = 0;  // must be accessed from one thread (reader, )

 private:
  ~client_session() {
    shutdown();
    server->opened_sessions.fetch_sub(1, std::memory_order_acq_rel);
  }

  dd::job start_writer();

 public:
  client_session(http2_server_ptr srv, client_session_ctx&& ctx, http2::settings_t client_opts,
                 http2::settings_t server_opts)
      : server(std::move(srv)),
        session_ctx(std::move(ctx)),
        client_settings(client_opts),
        decoder(client_opts.header_table_size),
        server_settings(server_opts),
        encoder(server_opts.header_table_size),
        framebuf(*this) {
    assert(server);
    server->opened_sessions.fetch_add(1, std::memory_order_acq_rel);
    start_writer();
  }

  friend void intrusive_ptr_add_ref(client_session* p) noexcept {
    ++p->refcount;
  }
  friend void intrusive_ptr_release(client_session* p) noexcept {
    --p->refcount;
    if (p == 0)
      delete p;
  }

  asio::ssl::stream<asio::ip::tcp::socket>& socket() noexcept {
    return session_ctx.socket;
  }

  [[nodiscard]] bool stop_requested() const noexcept {
    return shutted_down || server->stop_requested();
  }

  void shutdown() noexcept;

  void receive_headers(http2::frame_header h, std::span<byte_t> data);
  void receive_data(http2::frame_header h, std::span<byte_t> data);
  void receive_rst_stream(http2::rst_stream);
  // TODO accept not stream id, but http2_stream*
  void end_stream(http2::stream_id_t id) {
    incomplete_responses.erase(id);
  }
  void end_stream_with_exception(http2::stream_id_t, std::exception_ptr);
  void receive_window_update(http2::window_update_frame);
  void receive_ping(http2::ping_frame frame);

  dd::job send_goaway_and_drop(http2::errc_e err, std::string debuginfo) try {
    client_session_ptr self = this;
    bytes_t bytes;
    on_scope_exit {
      shutdown();
    };
    // ignore possible bad alloc
    http2::goaway_frame::form(last_opened_stream, err, std::move(debuginfo), std::back_inserter(bytes));
    io_error_code ec;
    co_await net.write(socket(), bytes, ec);
    if (ec)
      LOG_ERR("cannot send goaway for {}", (void*)this);
  } catch (std::bad_alloc&) {
    // ignore and just shutdown
  }
};

auto frames_buffer::next_work() {
  struct next_work_awaiter {
    frames_buffer& self;

    bool await_ready() noexcept {
      return self.session->stop_requested() || !self.bytes_to_send.empty();
    }
    void await_suspend(std::coroutine_handle<> h) noexcept {
      assert(!self.writer && "session must have exactly one writer");
      self.writer = h;
    }
    void await_resume() noexcept {
      self.writer = nullptr;
    }
  };
  return next_work_awaiter(*this);
}

dd::job client_session::start_writer() {
  client_session_ptr self = this;
  io_error_code ec;
  for (;;) {
    assert(self->session_ctx.exe.running_in_this_thread());
    co_await framebuf.next_work();
    if (self->stop_requested()) {
      LOG_DEBUG("[HTTP2]: writer for session {} ended after stop request", (void*)this);
      co_return;
    }
    bytes_t bytes = std::move(framebuf.bytes_to_send);
    co_await net.write(self->socket(), bytes, ec);
    if (ec && ec != asio::error::operation_aborted) {
      LOG_ERR("[HTTP2]: writer for session {} ended after network err: {}", (void*)this, ec.what());
      co_return;
    }
  }
  unreachable();
}

void frames_buffer::push_ping_answer(http2::ping_frame ping) {
  assert(!(ping.header.flags & http2::flags::ACK));
  ping.header.flags = http2::flags::ACK;
  push_frame(ping.header, ping.data);
  commit();
}

void frames_buffer::push_headers(http2::stream_id_t streamid, std::span<const byte_t> hdrs, bool has_data) {
  // do not respect window sizes, because its headers
  using namespace http2::flags;
  using enum http2::frame_e;

  const uint32_t max_frame_sz = session->client_settings.max_frame_size;
  const size_t hdrs_sz = hdrs.size();
  uint32_t frame_sz = (uint32_t)std::min<size_t>(hdrs_sz, max_frame_sz);
  http2::flags_t flags = EMPTY_FLAGS;
  if (hdrs_sz == frame_sz)
    flags |= END_HEADERS;
  // note: it may be END_STREAM, but not END_HEADERS,
  // continuations may be sent even after END_STREAM headers frame
  if (!has_data)
    flags |= END_STREAM;
  http2::frame_header framehdr{
      .length = frame_sz,
      .type = HEADERS,
      .flags = flags,
      .stream_id = streamid,
  };
  push_frame(framehdr, prefix(std::span(hdrs), frame_sz));
  if (hdrs_sz != frame_sz) [[unlikely]] {
    framehdr.type = CONTINUATION;
    framehdr.flags = EMPTY_FLAGS;
    for (size_t unhandled = hdrs_sz - frame_sz; unhandled != 0; unhandled -= frame_sz) {
      frame_sz = (uint32_t)std::min<size_t>(unhandled, max_frame_sz);
      framehdr.length = frame_sz;
      if (frame_sz == unhandled)
        framehdr.flags = END_HEADERS;
      push_frame(framehdr, std::span(hdrs).subspan(hdrs_sz - unhandled, frame_sz));
    }
  }
}

void frames_buffer::push_data(http2::stream_id_t streamid, std::span<const byte_t> data) {
  // ignores receiver control flow
  auto max_frame_sz = [&]() -> uint32_t {
    return std::min({session->receiver_window_size, session->client_settings.max_frame_size});
  };
  uint32_t frame_sz;
  const size_t data_sz = data.size();
  assert(data_sz != 0);
  for (size_t unhandled = data_sz; unhandled != 0; unhandled -= frame_sz) {
    frame_sz = (uint32_t)std::min<size_t>(unhandled, max_frame_sz());
    http2::frame_header framehdr{
        .length = frame_sz,
        .type = http2::frame_e::DATA,
        .flags = unhandled == frame_sz ? http2::flags::END_STREAM : http2::flags::EMPTY_FLAGS,
        .stream_id = streamid,
    };
    push_frame(framehdr, data.subspan(data_sz - unhandled, frame_sz));
  }
}

static bytes_t encode_response_headers(client_session& ses, const http_response& rsp) {
  hpack::encoder& encoder = ses.encoder;
  bytes_t hdrs;
  auto out = std::back_inserter(hdrs);
  using enum hpack::static_table_t::values;
  if (ses.table_size_decrease) {
    uint32_t dec = ses.table_size_decrease;
    ses.table_size_decrease = 0;
    LOG("[HTTP2]: reducing table size after receiving SETTINGS, old size: {}, decrease: {}",
        encoder.dyntab.max_size(), dec);

    if (encoder.dyntab.max_size() - dec)
      throw protocol_error{};
    encoder.encode_dynamic_table_size_update(encoder.dyntab.max_size() - dec, out);
  }

  switch (rsp.status) {
#define TGBM_ENCODE_STATUS_CASE(NMB)                        \
  case NMB:                                                 \
    encoder.encode_header_fully_indexed(status_##NMB, out); \
    break

    TGBM_ENCODE_STATUS_CASE(200);
    TGBM_ENCODE_STATUS_CASE(204);
    TGBM_ENCODE_STATUS_CASE(206);
    TGBM_ENCODE_STATUS_CASE(304);
    TGBM_ENCODE_STATUS_CASE(400);
    TGBM_ENCODE_STATUS_CASE(404);
    TGBM_ENCODE_STATUS_CASE(500);
#undef TGBM_ENCODE_STATUS_CASE
    default:
      char int_rep[32];
      auto [ptr, ec] = std::to_chars(int_rep, std::end(int_rep), rsp.status);
      assert(ec == std::errc{});
      encoder.encode(status_200, std::string_view(int_rep, ptr), out);
  }
  hpack::encode_headers_block(encoder, rsp.headers, out);

  return hdrs;
}

void frames_buffer::push_response(http2::stream_id_t streamid, http_response&& rsp) {
  push_headers(streamid, encode_response_headers(*session, rsp), !rsp.body.empty());
  if (!rsp.body.empty())
    push_data(streamid, rsp.body);
  commit();
}

void client_session::shutdown() noexcept {
  shutted_down = true;
  if (shutted_down || !socket().lowest_layer().is_open())
    return;
  io_error_code ec;
  ec = socket().lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
  if (ec)
    LOG_ERR("error while shutting down session (tcp part) {}", (void*)this);
  ec = socket().shutdown(ec);
  if (ec)
    LOG_ERR("error while shutting down session {}", (void*)this);
}

void client_session::receive_window_update(http2::window_update_frame frame) {
  if (frame.header.stream_id == 0) {
    http2::increment_window_size(receiver_window_size, frame.window_size_increment);
    return;
  }
  if (frame.header.stream_id > last_opened_stream) {
    LOG_WARN("unplemented stream window size increment before stream is open");
    return;
  }
  auto it = incomplete_responses.find(frame.header.stream_id);
  if (it == incomplete_responses.end())
    return;
  http2::increment_window_size(it->second.streamlevel_window_size, frame.window_size_increment);
}

void client_session::receive_ping(http2::ping_frame frame) {
  if (stop_requested() || frame.header.flags & http2::flags::ACK)
    return;
  framebuf.push_ping_answer(frame);
}

void client_session::receive_rst_stream(http2::rst_stream rst) {
  bool erased = incomplete_requests.erase(rst.header.stream_id);
  LOG_DEBUG("rst stream {}, {}", rst.header.stream_id, erased ? "erased" : "not erased");
}

// TODO debug info only under special flag (ifdef)
void client_session::receive_headers(http2::frame_header h, std::span<byte_t> data) {
  // validate
  if (!(h.flags & http2::flags::END_HEADERS))
    throw unimplemented("CONTINUATION frame");
  if (h.stream_id <= last_opened_stream) {
    throw http2::goaway_exception(last_opened_stream, http2::errc_e::STREAM_CLOSED,
                                  fmt::format("incorrect ordering of streams, received {}, last opened {}",
                                              h.stream_id, last_opened_stream));
  }
  last_opened_stream = h.stream_id;

  // create new request

  http_request req;

  http2::parse_http2_request_headers(decoder, data, req);

  if (h.flags & http2::flags::END_STREAM) {
    start_handle_request(this, h.stream_id, std::move(req));
  } else {
    auto [it, emplaced] = incomplete_requests.try_emplace(h.stream_id, std::move(req));
    assert(emplaced);
    (void)emplaced;
  }
}
// TODO signed размер сделать, потому что клиент может залезать в долги при отправке фреймов (например
// отправка до SETTINGS)
void client_session::receive_data(http2::frame_header h, std::span<byte_t> data) {
  assert(h.type == http2::frame_e::DATA);
  if (my_window_size < h.length) {
    // TODO какую-то настройку или хз, ну по сути тестовый сервер, так что он проверяет всё strict...
    send_goaway_and_drop(
        http2::errc_e::FLOW_CONTROL_ERROR,
        fmt::format("too many DATA sended, can accept {}, received frame {}", my_window_size, h));
    // TODO что делать, чтобы не плодить тыщу таких фреймов пока не отправится?
    // TODO control flow для стрима
    return;
  }
  if (h.flags & http2::flags::PADDED)
    http2::strip_padding(data);
  // TODO track stream id order?
  auto it = incomplete_requests.find(h.stream_id);
  if (it != incomplete_requests.end()) {
    if (it->second.headers.empty()) {
      throw http2::goaway_exception(
          last_opened_stream, http2::errc_e::PROTOCOL_ERROR,
          fmt::format("incorrect stream ordering, {} stream DATA sended before HEADERS", h.stream_id));
    }
    it->second.body.data.insert(it->second.body.data.end(), data.begin(), data.end());
    if (h.flags & http2::flags::END_STREAM)
      start_handle_request(this, it->first, std::move(it->second));
  }
}

void client_session::end_stream_with_exception(http2::stream_id_t streamid, std::exception_ptr) {
  // TODO rethrow + different codes?
  http_response rsp;
  rsp.status = 500;
  framebuf.push_response(streamid, std::move(rsp));
  end_stream(streamid);
}

// TODO http2 stream вместо streamid, чтобы контрол флоу и тд
dd::job start_handle_request(client_session_ptr session, http2::stream_id_t streamid, http_request req) {
  assert(session);
  dd::task t = session->server->handle_request(std::move(req));
  co_await t.wait();
  if (session->stop_requested())
    co_return;
  if (t.raw_handle().promise().exception) [[unlikely]] {
    session->end_stream_with_exception(streamid, std::move(t.raw_handle().promise().exception));
    co_return;
  }
  http_response rsp = t.raw_handle().promise().result();
  assert(rsp.status != 0 && "status required to set");
  session->framebuf.push_response(streamid, std::move(rsp));
  session->end_stream(streamid);
}

http2_server::http2_server(http2_server_options opts, tcp_connection_options tcpopts,
                           size_t listen_thread_count)
    : options(std::move(opts)),
      tcp_options(tcpopts),
      sslctx(make_ssl_context_for_server(options.ssl_cert_path, options.private_key_path)),
      tp(listen_thread_count) {
  if (!sslctx)
    throw std::runtime_error("[HTTP2] server cannot initialize sslctx! Set path to both files in options!");
  // TODO таймер на коннекшн и таймер на получение следующего запроса (фрейм хедер + данные)
  options.initial_stream_window_size = std::min(options.initial_stream_window_size, http2::max_window_size);
  options.max_concurrent_stream_per_connection = std::clamp<uint32_t>(
      options.max_concurrent_stream_per_connection, 1, http2::settings_t::max_max_concurrent_streams);
  options.max_receive_frame_size =
      std::clamp<uint32_t>(options.max_receive_frame_size, 1, http2::frame_len_max);
  options.max_send_frame_size = std::clamp<uint32_t>(options.max_send_frame_size, 1, http2::frame_len_max);
}

dd::job reader_for(client_session_ptr session) try {
  co_await std::suspend_always{};
  assert(session);
  using namespace http2;

  io_error_code ec;
  reusable_buffer buffer;

  constexpr uint32_t h2fhl = frame_header_len;

  auto validate_frame_header = [](frame_header h) -> bool {
    return h.length <= frame_len_max && h.stream_id <= max_stream_id &&
           (h.stream_id == 0 || (h.stream_id % 2) == 1);
  };

  for (;;) {
    if (session->stop_requested())
      co_return;

    // read frame header

    std::span buf = buffer.get_exactly(h2fhl);

    co_await net.read(session->socket(), buf, ec);

    if (ec)
      goto network_error;
    if (session->stop_requested())
      co_return;

    // parse frame header

    http2::frame_header framehdr = http2::frame_header::parse(buf);
    if (!validate_frame_header(framehdr)) {
      throw goaway_exception(session->last_opened_stream, errc_e::PROTOCOL_ERROR,
                             fmt::format("invalid frame header: {}", framehdr));
    }

    // read frame data

    buf = buffer.get_exactly(framehdr.length);
    co_await net.read(session->socket(), buf, ec);
    if (ec)
      goto network_error;
    if (session->stop_requested())
      co_return;

    // handle frame
    using enum http2::frame_e;
    using namespace http2::flags;
    switch (framehdr.type) {
      case DATA:
        session->receive_data(framehdr, buf);
        break;
      case HEADERS:
        session->receive_headers(framehdr, buf);
        break;
      case RST_STREAM:
        session->receive_rst_stream(http2::rst_stream::parse(framehdr, buf));
        break;
      case SETTINGS: {
        client_settings_visitor vtor{session->client_settings};
        settings_frame::parse(framehdr, buf, vtor);
        if (vtor.header_table_size_decrease)
          session->table_size_decrease += vtor.header_table_size_decrease;
        break;
      }
      case PUSH_PROMISE:
        throw goaway_exception(
            session->last_opened_stream, errc_e::PROTOCOL_ERROR,
            fmt::format("client must not send PUSH_PROMISE, received in stream {}", framehdr.stream_id));
      case PING:
        session->receive_ping(ping_frame::parse(framehdr, buf));
        break;
      case GOAWAY: {
        goaway_frame f = goaway_frame::parse(framehdr, buf);
        LOG_ERR("[HTTP2]: server receives goaway frame, errc: {}, debuginfo: {}", e2str(f.error_code),
                f.debug_info);
        session->shutdown();
        co_return;
      }
      case WINDOW_UPDATE:
        session->receive_window_update(window_update_frame::parse(framehdr, buf));
        break;
      case CONTINUATION:
        throw unimplemented(
            "CONTINUATION frame, which must not be sended, because error must be handled at HEADER frame "
            "level");
      default:
      case PRIORITY:
      case PRIORITY_UPDATE:
        // ignore
        break;
    }
    // TODO на уровне стримов тоже надо при получении данных увеличивать окно
    // TODO ещё в начале нужно правильный установить хотя бы размер стриму
    if (session->my_window_size < http2::max_window_size / 2) {
      uint32_t inc = http2::max_window_size - session->my_window_size;
      byte_t bytes[window_update_frame::len];
      window_update_frame::form(0, inc, bytes);
      co_await net.write(session->socket(), bytes, ec);
      if (ec)
        goto network_error;
      session->my_window_size += inc;
    }
  }
network_error:
  if (ec != asio::error::operation_aborted)
    LOG_ERR("[HTTP2]: client session ended after network error: {}", ec.what());
  session->shutdown();
  co_return;
} catch (http2::hpack_error& e) {
  session->send_goaway_and_drop(http2::errc_e::COMPRESSION_ERROR, e.what());
} catch (protocol_error& e) {
  session->send_goaway_and_drop(http2::errc_e::PROTOCOL_ERROR, e.what());
} catch (http2::goaway_exception& e) {
  session->send_goaway_and_drop(e.error_code, std::move(e.debug_info));
} catch (unimplemented& e) {
  session->send_goaway_and_drop(http2::errc_e::NO_ERROR, fmt::format("unimplemented {}", e.what()));
} catch (...) {
  session->send_goaway_and_drop(http2::errc_e::INTERNAL_ERROR, "unknown server error");
}

// simple minimal connection establishment
dd::job establish_client_session(http2_server_ptr server, client_session_ctx session_ctx) try {
  io_error_code ec;
  using namespace http2;
  constexpr size_t preface_sz = std::size(http2::connection_preface);

  reusable_buffer buf;
  buf.reserve(preface_sz + frame_header_len + 10 * sizeof(setting_t) + 4096);
  // TODO timeout for connection here
  {  // read preface
    std::span preface = buf.get_exactly(preface_sz);
    co_await net.read(session_ctx.socket, preface, ec);
    if (ec) {
      LOG_ERR("[HTTP2]: client session establishment failed, err: {}", ec.what());
      co_return;
    }
    if (!std::ranges::equal(preface, std::span(http2::connection_preface))) {
      LOG_ERR("[HTTP2]: incorrect client connection for http2, cannot establish session");
      co_return;
    }
  }

  if (server->stop_requested())
    co_return;

  frame_header settings_header;
  {  // read settings frame
    std::span settings_frame = buf.get_exactly(http2::frame_header_len);
    co_await net.read(session_ctx.socket, settings_frame, ec);
    if (ec) {
      LOG_ERR("[HTTP2]: client session establishment failed, err: {}", ec.what());
      co_return;
    }
    settings_header = frame_header::parse(settings_frame);
  }
  if (settings_header.type != http2::frame_e::SETTINGS) {
    LOG_ERR("[HTTP2]: client session establishment failed: incorrect client preface, frame is not SETTINGS");
    co_return;
  }

  if (server->stop_requested())
    co_return;

  settings_t client_settings;
  {  // read settings data
    std::span settings_data = buf.get_exactly(settings_header.length);
    co_await net.read(session_ctx.socket, settings_data, ec);
    if (ec) {
      LOG_ERR("[HTTP2]: client session establishment failed, err: {}", ec.what());
      co_return;
    }
    settings_frame::parse(settings_header, settings_data, client_settings_visitor(client_settings));
  }

  if (server->stop_requested())
    co_return;
  // write ACK and my settiings

  auto& server_opts = server->get_options();
  settings_t mysettings{
      .header_table_size = server_opts.hpack_dyntab_size,
      .max_concurrent_streams = server_opts.max_concurrent_stream_per_connection,
      .initial_stream_window_size = server_opts.initial_stream_window_size,
      .max_frame_size = server_opts.max_receive_frame_size,
      .deprecated_priority_disabled = true,
  };
  {
    std::vector<byte_t> bytes;
    settings_frame::form(mysettings, std::back_inserter(bytes));
    accepted_settings_frame().send_to(std::back_inserter(bytes));
    co_await net.write(session_ctx.socket, bytes, ec);
    if (ec) {
      LOG_ERR("[HTTP2]: client session establishment failed: cannot send ACK frame to client, err: {}",
              ec.what());
      co_return;
    }
  }
  // TODO мб здесь это всё не нужно и автоматически будет завершаться с ошибкой (ec) если закрыто
  if (server->stop_requested())
    co_return;

  // start new client session

  dd::job reader =
      reader_for(new client_session(std::move(server), std::move(session_ctx), client_settings, mysettings));
  co_await dd::this_coro::destroy_and_transfer_control_to(reader.handle);
} catch (protocol_error& pe) {
  LOG_ERR("[HTTP2]: client session failed: {}", pe.what());
} catch (...) {
  LOG_ERR("[HTTP2]: client session failed with unknown exception");
}

dd::job http2_server::start_accept(asio::ip::tcp::endpoint ep) {
  using tcp = asio::ip::tcp;
  // prevent deleting
  http2_server_ptr self = this;

  tcp::acceptor acceptor(io_ctx, ep);
  io_error_code ec;
  for (;;) {
    client_session_ctx session_ctx{
        .exe = asio::make_strand(io_ctx),
        .socket = asio::ssl::stream<tcp::socket>(tcp::socket(session_ctx.exe), sslctx->ctx),
    };
    LOG_DEBUG("[HTTP2] starting accept!");
    co_await net.accept(acceptor, session_ctx.socket.lowest_layer(), ec);

    if (ec) {
      if (ec != asio::error::operation_aborted)
        LOG_ERR("[HTTP2]: server stops accepting after error: {}", ec.what());
      co_return;
    }
    LOG_DEBUG("[HTTP2] accepted TCP socket");

    co_await net.handshake(session_ctx.socket, asio::ssl::stream_base::server, ec);
    if (ec) {
      if (ec == asio::error::operation_aborted)
        co_return;
      LOG_ERR("[HTTP2]: server cannot accept (ssl handshake failure), error: {}", ec.what());
      // do not stop accepting
    }
    if (stop_requested())
      co_return;

    LOG_DEBUG("[HTTP2]: establishing client session");

    establish_client_session(this, std::move(session_ctx));
  }
}

void http2_server::listen(asio::ip::tcp::endpoint ep) {
  start_accept(ep);  // start accept
}

bool http2_server::start() {
  if (work_guard)
    return false;
  if (io_ctx.stopped()) {
    // drop old sessions before starting new ones
    io_ctx.restart();
    while (opened_sessions.load(std::memory_order_acq_rel) != 0)
      io_ctx.run_one();
  }
  work_guard = std::shared_ptr<work_guard_t>(new auto(asio::make_work_guard(io_ctx)));
  for (dd::worker& w : tp.workers_range())
    dd::schedule_to(w, [&ctx = this->io_ctx, g = work_guard] { ctx.run(); });
  return true;
}

void http2_server::run() {
  if (start())
    io_ctx.run();
}

void http2_server::stop() {
  if (!work_guard)
    return;
  _stop_requested.store(true);
  http2_server_ptr self = this;
  (*work_guard).reset();
  work_guard = nullptr;
  io_ctx.stop();
}

}  // namespace tgbm
