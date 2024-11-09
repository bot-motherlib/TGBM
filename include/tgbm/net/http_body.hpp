#pragma once

#include <string>
#include <cassert>
#include <vector>
#include <iterator>

#include "tgbm/tools/macro.hpp"
#include "tgbm/tools/StringTools.h"
#include "tgbm/tools/rapidjson_to_json.h"
#include "tgbm/api/input_file.hpp"

#include <fmt/format.h>

namespace tgbm {

using bytes_t = std::vector<uint8_t>;

struct http_body {
  std::string content_type;
  bytes_t data;
};

struct rj_refbuffer_t {
  bytes_t& buf;

  using Ch = char;

  void Put(char c) {
    buf.push_back(c);
  }
  static void Flush() noexcept {
  }
};

struct rj_urlencoded_refbuffer_t {
  bytes_t& buf;

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
    bytes_t buf;

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
  http_body take() noexcept {
    if (body.buf.size() == 1) {
      assert(body.buf.front() == '{');
      body.buf.clear();
    } else
      rj_assume(writer.EndObject());
    return http_body{
        .content_type = "application/json",
        .data = std::move(body.buf),
    };
  }

  void arg(std::string_view k, const auto& value) {
    writer.Key(k.data(), k.size());
    rj_tojson(writer, value);
  }
};
// TODO test with complex objects and arrays
struct application_x_www_form_urlencoded {
 private:
  bytes_t body;

 public:
  application_x_www_form_urlencoded() = default;
  explicit application_x_www_form_urlencoded(size_t reserve) {
    body.reserve(reserve);
  }
  http_body take() noexcept {
    if (!body.empty()) {
      assert(body.back() == '&');
      body.pop_back();
    }
    return http_body{
        .content_type = "application/x-www-form-urlencoded",
        .data = std::move(body),
    };
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
  bytes_t body;

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

  http_body take() noexcept {
    if (!body.empty())
      fmt::format_to(std::back_inserter(body), "\r\n--{}--\r\n", boundary);
    return http_body{
        .content_type = fmt::format("multipart/form-data; boundary=\"{}\"", get_boundary()),
        .data = std::move(body),
    };
  }

  // only this body type supports files (TG only supports this content type for sending files)
  void arg(std::string_view key, const api::InputFile& f) {
    file_arg(key, f.filename, f.mimetype, f.data);
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
    body.push_back('\r');
    body.push_back('\n');
  }
};

}  // namespace tgbm
