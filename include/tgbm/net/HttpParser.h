#pragma once

#include <string>
#include <unordered_map>
#include <cassert>

#include "tgbm/tools/StringTools.h"
#include "tgbm/tools/rapidjson_to_json.h"

#include <fmt/format.h>

namespace tgbm {

struct tg_url_view {
  std::string_view host;
  std::string_view path;  // path without method part, e.g. "/bot123/"
  std::string_view method;
};

// TODO wtf это всё статические функции вообще
struct HttpParser {
  std::string generateResponse(const std::string& data, const std::string& mimeType,
                               unsigned short statusCode, const std::string& statusStr,
                               bool isKeepAlive) const;
  std::unordered_map<std::string, std::string> parseHeader(const std::string& data, bool isRequest) const;
};

struct rj_refbuffer_t {
  std::string& buf;  // TODO replace with vector

  using Ch = char;

  void Put(char c) {
    buf.push_back(c);
  }
  static void Flush() noexcept {
  }
};

struct rj_urlencoded_refbuffer_t {
  std::string& buf;  // TODO replace with vector

  using Ch = char;

  void Put(char c) {
    utils::url_encode(c, std::back_inserter(buf));
  }
  static void Flush() noexcept {
  }
};

struct application_json_body {
 private:
  struct buffer_type {
    std::string buf;  // TODO replace with vector

    using Ch = char;

    void Put(char c) {
      buf.push_back(c);
    }
    static void Flush() noexcept {
    }
  };

  buffer_type body;
  rapidjson::Writer<buffer_type> writer;

 public:
  explicit application_json_body(size_t reserve = 0) : writer(body) {
    body.buf.reserve(reserve);
    rj_assume(writer.StartObject());
  }
  std::string take() noexcept {
    if (body.buf.size() == 1) {
      assert(body.buf.front() == '{');
      body.buf.clear();
    } else
      rj_assume(writer.EndObject());
    return std::move(body.buf);
  }

  void arg(std::string_view k, const auto& value) {
    writer.Key(k.data(), k.size());
    rj_tojson(writer, value);
  }
};
// TODO test with complex objects and arrays
struct application_x_www_form_urlencoded {
 private:
  std::string body;

 public:
  application_x_www_form_urlencoded() = default;
  explicit application_x_www_form_urlencoded(size_t reserve) {
    body.reserve(reserve);
  }
  std::string take() noexcept {
    if (!body.empty()) {
      assert(body.back() == '&');
      body.pop_back();
    }
    return std::move(body);
  }

  void arg(std::string_view k, const auto& value) {
    fmt::format_to(std::back_inserter(body), "{}", utils::url_encoded(k));
    body.push_back('=');
    rj_urlencoded_refbuffer_t b(body);
    using namespace rapidjson;
    constexpr auto flag = std::is_convertible_v<decltype(value), std::string_view>
                              ? WriteFlag::kWriteUnquotedString
                              : WriteFlag::kWriteDefaultFlags;
    Writer<rj_urlencoded_refbuffer_t, UTF8<>, UTF8<>, CrtAllocator, flag> writer(b);
    rj_tojson(writer, value);
    body.push_back('&');
  }
};

// just hopes that 'boundary' is not contained in arguments
// TODO test with complex object and arrays
struct application_multipart_form_data {
 private:
  std::string boundary;
  std::string body;

 public:
  explicit application_multipart_form_data(size_t reserve = 0) {
    boundary = utils::generate_multipart_boundary(16);
    body.reserve(reserve);
  }
  explicit application_multipart_form_data(std::string boudary, size_t reserve = 0) {
    if (boundary.size() > 69)
      throw std::invalid_argument("HTTP boundary should be [1-69] symbols");
    if (boudary.empty())
      boudary = utils::generate_multipart_boundary(16);
    body.reserve(reserve);
  }

  std::string_view get_boundary() const noexcept {
    return boundary;
  }

  std::string take() noexcept {
    if (!body.empty())
      fmt::format_to(std::back_inserter(body), "\r\n--{}--\r\n", boundary);
    return std::move(body);
  }

  void file_arg(std::string_view k, std::string_view filename, std::string_view mime_type,
                std::string_view data) {
    fmt::format_to(std::back_inserter(body),
                   "--{}\r\n"
                   "Content-Disposition: form-data; name=\"{}\"; filename=\"{}\"\r\n"
                   "Content-Type: {}\r\n\r\n"
                   "{}\r\n",
                   boundary, k, utils::url_encoded(filename), mime_type, data);
  }

  void arg(std::string_view k, const auto& value) {
    constexpr std::string_view content_type =
        std::is_convertible_v<decltype(value), std::string_view> ? "text/plain" : "application/json";
    fmt::format_to(std::back_inserter(body),
                   "--{}\r\n"
                   "Content-Disposition: form-data; name=\"{}\"\r\n"
                   "Content-Type: {}\r\n\r\n",
                   boundary, k, content_type);
    rj_refbuffer_t b(body);
    using namespace rapidjson;
    constexpr auto flag =
        content_type == "text/plain" ? WriteFlag::kWriteUnquotedString : WriteFlag::kWriteDefaultFlags;
    Writer<rj_refbuffer_t, UTF8<>, UTF8<>, CrtAllocator, flag> writer(b);
    rj_tojson(writer, value);
    body += "\r\n";
  }
};

std::string generate_http_headers_get(const tg_url_view& url, bool keep_alive);
// GET if body.empty(), POST otherwise
std::string generate_http_headers(const tg_url_view& url, bool keep_alive, std::string_view body,
                                  std::string_view content_type);

struct http_request {
  std::string body;
  std::string headers;  // TODO vectors (?allocs?)

  // TODO rm url/keep alive
  http_request(tg_url_view url, application_json_body&& b, bool keep_alive = true)
      : body(b.take()), headers(generate_http_headers(url, keep_alive, body, "application/json")) {
  }
  http_request(tg_url_view url, application_x_www_form_urlencoded&& b, bool keep_alive = true)
      : body(b.take()),
        headers(generate_http_headers(url, keep_alive, body, "application/x-www-form-urlencoded")) {
  }
  http_request(tg_url_view url, application_multipart_form_data&& b, bool keep_alive = true)
      : body(b.take()),
        headers(generate_http_headers(
            // https://datatracker.ietf.org/doc/html/rfc2046#section-5.1.1
            url, keep_alive, body, fmt::format("multipart/form-data; boundary=\"{}\"", b.get_boundary()))) {
  }

  // GET request
  explicit http_request(tg_url_view url, bool keep_alive = true)
      : headers(generate_http_headers_get(url, keep_alive)) {
  }
};

struct http_response {
  std::string body;
  std::string headers;  // TODO vectors (?allocs?)
  // TODO parse status string (error code, msg, http version)
};

}  // namespace tgbm
