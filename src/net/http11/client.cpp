
#include <tgbm/net/http11/client.hpp>

#include <charconv>
#include <cstddef>

#include <tgbm/net/asio/asio_awaiters.hpp>

#include <tgbm/net/errors.hpp>

namespace tgbm {

// placeholder
static void parse_status_string(std::string_view& in, int& status) {
  std::string_view tok;
  auto eat_until = [&](std::string_view until) {
    auto pos = in.find(until);
    if (pos == in.npos)
      throw protocol_error{};
    tok = in.substr(0, pos);
    in.remove_prefix(pos + until.length());
  };
  eat_until(" ");  // version
  eat_until(" ");  // status code
  if (tok.size() != 3)
    throw protocol_error{};
  auto [_, err] = std::from_chars(tok.data(), tok.data() + tok.size(), status);
  if (err != std::errc{})
    throw protocol_error{};
  eat_until("\r\n");
}
// placeholder
static std::pair<std::string_view, std::string_view> parse_header(std::string_view& in) {
  std::string_view tok;
  auto eat_until = [&](std::string_view until) {
    auto pos = in.find(until);
    if (pos == in.npos)
      throw protocol_error{};
    tok = in.substr(0, pos);
    in.remove_prefix(pos + until.length());
  };
  eat_until(":");  // header
  std::pair<std::string_view, std::string_view> result;
  result.first = tok;
  while (!in.empty() && in.front() == ' ')
    in.remove_prefix(1);
  if (in.empty())
    throw protocol_error{};
  eat_until("\r\n");
  while (!tok.empty() && tok.back() == ' ')
    tok.remove_suffix(1);
  if (tok.empty())
    throw protocol_error{};
  result.second = tok;
  return result;
}

static std::string generate_http_headers(const http_request& request, std::span<const uint8_t> body,
                                         std::string_view content_type) {
  assert(!request.path.empty());
  std::string body_sz_str = fmt::format("{}", body.size());
  size_t size_bytes = [&] {
    using namespace std::string_view_literals;
    size_t headers_size = 0;
    headers_size += e2str(request.method).size() + 1;
    headers_size += request.path.size();
    headers_size += " HTTP/1.1\r\nHost: "sv.size() + request.authority.size() + "\r\nConnection: "sv.size() +
                    "keep-alive"sv.size();
    headers_size += "\r\n"sv.size();
    if (body.empty()) {
      headers_size += "\r\n"sv.size();
      return headers_size;
    }
    headers_size += "Content-Type: "sv.size() + content_type.size() + "\r\nContent-Length: "sv.size() +
                    body_sz_str.size() + "\r\n\r\n"sv.size();
    for (auto& [name, value] : request.headers)
      headers_size += name.size() + value.size() + 4;  // 4 == : \r\n and whitespace
    return headers_size;
  }();
  std::string result;
  result.reserve(size_bytes);
  on_scope_exit {
    assert(result.size() == size_bytes && "not exactly equal size reserved, logical error");
  };
  result += e2str(request.method);
  result.push_back(' ');
  result += request.path;
  result +=
      " HTTP/1.1\r\n"
      "Host: ";
  result += request.authority;
  result += "\r\nConnection: ";
  result += "keep-alive";
  result += "\r\n";
  for (auto& [name, value] : request.headers) {
    result += name;
    result += ": ";
    result += value;
    result += "\r\n";
  }
  if (body.empty()) {
    result += "\r\n";
    return result;
  }
  result += "Content-Type: ";
  result += content_type;
  result += "\r\nContent-Length: ";
  result += body_sz_str;
  result += "\r\n\r\n";
  return result;
}

static dd::task<void> send_request(tcp_tls_connection* con, on_header_fn_ptr on_header,
                                   on_data_part_fn_ptr on_data_part, std::string headers,
                                   http_request& request, int& status) try {
  namespace asio = boost::asio;
  using tcp = asio::ip::tcp;
  assert(con);
  auto& socket = con->socket;
  io_error_code ec;
  std::string::size_type headers_end_pos = 0;
  size_t transfered = co_await net.write_many(socket, ec, std::span(headers), std::span(request.body.data));
  if (ec)
    goto network_err;
  assert(transfered == (headers.size() + request.body.data.size()));
  if (!on_header && !on_data_part) {
    status = reqerr_e::done;
    co_return;
  }
  headers.clear();

  static constexpr std::string_view headers_end_marker = "\r\n\r\n";
  // read http headers
  headers_end_pos = co_await net.read_until(socket, headers, headers_end_marker, ec);
  if (ec)
    goto network_err;
  {
    std::string_view headers_data =
        std::string_view(headers.data(), headers_end_pos + headers_end_marker.size());
    parse_status_string(headers_data, status);
    if (on_header) {
      while (headers_data != "\r\n") {
        auto [name, value] = parse_header(headers_data);
        (*on_header)(name, value);
      }
    }
  }
  if (!on_data_part)
    co_return;  // status setted by parsing headers
  static constexpr std::string_view content_length_header = "Content-Length:";
  {
    auto content_len_pos = headers.find(content_length_header);
    if (content_len_pos == headers.npos)
      goto protocol_err;
    content_len_pos += content_length_header.size();
    // skip whitespaces (exactly one usually)
    content_len_pos = headers.find_first_not_of(' ', content_len_pos);
    assert(content_len_pos < headers_end_pos);

    size_t content_length;
    auto [_, err] =
        std::from_chars(headers.data() + content_len_pos, headers.data() + headers_end_pos, content_length);
    if (err != std::errc{})
      goto protocol_err;

    headers_end_pos += headers_end_marker.size();
    size_t overriden_bytes = headers.size() - headers_end_pos;
    assert(overriden_bytes <= content_length);
    // store overriden body and erase it from headers
    request.body.data.resize(content_length);
    memcpy(request.body.data.data(), headers.data() + headers_end_pos, overriden_bytes);
    headers.erase(headers.begin() + headers_end_pos, headers.end());
    transfered = co_await net.read(
        socket, std::span(request.body.data.data() + overriden_bytes, content_length - overriden_bytes), ec);
    if (ec)
      goto network_err;
    assert(transfered == content_length - overriden_bytes);
  }
  (*on_data_part)(request.body.data, true);
  co_return;  // status setteed by parsing headers
network_err:
  assert(!!ec);
  if (ec == asio::error::operation_aborted)
    status = reqerr_e::timeout;
  else
    status = reqerr_e::network_err;
  co_return;
protocol_err:
  status = reqerr_e::protocol_err;
  co_return;
} catch (protocol_error& e) {
  status = reqerr_e::protocol_err;
  co_return;
} catch (...) {
  status = reqerr_e::user_exception;
  throw;
}

http11_client::http11_client(size_t connections_max_count, std::string_view host,
                             tcp_connection_options tcp_opts)
    : http_client(host), io_ctx(1), tcp_options(tcp_opts), connections(connections_max_count, [this]() {
        // Do not reuses ssl ctx because... just because + multithread no one knows how to work
        return tcp_tls_connection::create(
            io_ctx, std::string(get_host()),
            make_ssl_context_for_http11(tcp_options.additional_ssl_certificates), deadline_t::never(),
            tcp_options);
      }) {
  if (tcp_options.disable_ssl_certificate_verify)
    TGBM_LOG_WARN("SSL veriication for http11 client disabled");
}

dd::task<int> http11_client::send_request(on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part,
                                          http_request request, deadline_t deadline) {
  if (stop_requested) [[unlikely]]
    co_return reqerr_e::cancelled;
  // TODO fully support deadlines
  if (deadline.is_reached()) [[unlikely]]
    co_return reqerr_e::timeout;
  ++requests_in_progress;
  on_scope_exit {
    --requests_in_progress;
  };
  auto handle = co_await connections.borrow();
  if (handle.empty())
    co_return reqerr_e::cancelled;
  if (request.authority.empty())
    request.authority = get_host();
  std::string headers = generate_http_headers(request, request.body.data, request.body.content_type);
  int status = reqerr_e::unknown_err;
  co_await ::tgbm::send_request(handle.get(), on_header, on_data_part, std::move(headers), request, status);
  // send request throws on user exception
  assert(status != reqerr_e::user_exception);
  if (status < 0 && status != reqerr_e::timeout) {
    handle.get()->shutdown();
    handle.drop();
  }
  co_return status;
}

void http11_client::run() {
  stop_requested = false;
  if (io_ctx.stopped())
    io_ctx.restart();
  io_ctx.run();
}

bool http11_client::run_one(duration_t timeout) {
  stop_requested = false;
  if (io_ctx.stopped())
    io_ctx.restart();
  auto count = io_ctx.run_one_for(timeout);
  return count > 0;
}

void http11_client::stop() {
  // avoids situations when stop() itself produces next stop()
  // for example in scope_exit on some coroutine which will be stopped while stopping
  if (stop_requested)
    return;
  // Note: stop_requested will not be reset until next .run called TODO recursvie run call?
  // this is required for correctly stopping even if someone just pushes next and next request
  // without stopping
  stop_requested = true;
  while (requests_in_progress != 0)
    io_ctx.run_one();
  connections.shutdown();
  while (!connections.empty())
    io_ctx.run_one();
  io_ctx.stop();
}

dd::task<void> http11_client::sleep(duration_t d, io_error_code& ec) {
  asio::steady_timer t(io_ctx);
  co_await net.sleep(t, d, ec);
}

}  // namespace tgbm
