#pragma once

#include "tgbm/net/Url.h"
#include "tgbm/net/HttpReqArg.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>

#include <rapidjson/writer.h>

namespace tgbm {

// TODO wtf это всё статические функции вообще
struct HttpParser {
  std::string generateRequest(const Url& url, const std::vector<HttpReqArg>& args,
                              bool isKeepAlive = false) const;
  static std::string generateApplicationJson(const std::vector<HttpReqArg>& args);
  std::string generateMultipartFormData(const std::vector<HttpReqArg>& args,
                                        const std::string& boundary) const;
  std::string generateMultipartBoundary(const std::vector<HttpReqArg>& args) const;
  std::string generateWwwFormUrlencoded(const std::vector<HttpReqArg>& args) const;
  std::string generateResponse(const std::string& data, const std::string& mimeType,
                               unsigned short statusCode, const std::string& statusStr,
                               bool isKeepAlive) const;
  std::unordered_map<std::string, std::string> parseHeader(const std::string& data, bool isRequest) const;
  std::string extractBody(const std::string& data) const;
};

// TODO в утилс хедер

template <typename T, typename... Types>
concept any_of = (std::is_same_v<T, Types> || ...);

template <typename T>
concept char_type_like = any_of<T, char, signed char, unsigned char, char8_t, char16_t, char32_t, wchar_t>;

template <typename T>
concept numeric = std::is_integral_v<T> && !char_type_like<T> && !std::is_same_v<T, bool>;

struct application_json_body {
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

  application_json_body() : writer(body) {
  }
#ifndef NDEBUG
  bool started : 1 = false;
  bool ended : 1 = false;
#endif
  // TODO чёт сделать с старт енд, мб в конструктор + .take какое то
  void start() {
    assert(!started && (started = true));
    rj_assume(writer.StartObject());
  }

  void end() {
    assert(started);
    assert(!ended && (ended = true));
    if (body.buf.size() == 1) {
      assert(body.buf.front() == '{');
      body.buf.clear();
      return;
    }
    rj_assume(writer.EndObject());
  }

  void arg(std::string_view k, const auto& value) {
    key(k);
    val(value);
  }
  template <std::ranges::range R>
  void arg(std::string_view k, const R& rng)
    requires(!std::convertible_to<R, std::string_view>)
  {
    key(k);
    writer.StartArray();
    for (auto&& item : rng)
      val(item);
    writer.EndArray();
  }

 private:
  void key(std::string_view k) {
    assert(started);
    // assumes our keys always ASCI encoded
    writer.Key(k.data(), k.size());
  }
  void val(bool b) {
    rj_assume(writer.Bool(b));
  }
  void val(std::string_view v) {
    rj_assume(writer.String(v.data(), v.size()));
  }
  void val(numeric auto numb) {
    using T = std::remove_cvref_t<decltype(numb)>;
    if constexpr (std::is_unsigned_v<T>) {
      if constexpr (sizeof(T) > sizeof(int32_t)) {
        rj_assume(writer.Uint64(numb));
      } else {
        rj_assume(writer.Uint(numb));
      }
    } else {
      if constexpr (sizeof(T) > sizeof(int32_t)) {
        rj_assume(writer.Int64(numb));
      } else {
        rj_assume(writer.Int(numb));
      }
    }
  }
  void val(char_type_like auto) = delete;
  // rapid json very bad handle doubles
  void val(long double) = delete;
  void val(double) = delete;
  void val(float) = delete;

  [[gnu::always_inline]] static void rj_assume(bool b) {
    assert(b);
    (void)b;
  }
};

struct application_x_www_form_urlencoded {
  // TODO
};
struct application_multipart_form_data {
  // TODO
};

template <typename BodyType>
struct http_content_type {};

template <>
struct http_content_type<application_json_body> {
  static consteval std::string_view value() {
    return "application/json";
  }
};

template <>
struct http_content_type<application_x_www_form_urlencoded> {
  static consteval std::string_view value() {
    return "application/x-www-form-urlencoded";
  }
};
// TODO для multipart/form-data; boundary= ещё нужно boundary прямо в контент тайп

std::string generate_http_headers_get(const Url& url, bool keep_alive);
// GET if !body.empty(), POST otherwise
std::string generate_http_headers(const Url& url, bool keep_alive, std::string_view body,
                                  std::string_view content_type);

struct http_request {
  std::string body;
  std::string headers;  // TODO vectors (?allocs?)

  // TODO overloads with application/x-www-form-urlencoded
  // and application/multipart/form-data (fixed/non-fixed boundary)
  // TODO rm url/keep alive
  explicit http_request(const Url& url, application_json_body&& b, bool keep_alive = true)
      : body(std::move(b.body.buf)),
        headers(
            generate_http_headers(url, keep_alive, body, http_content_type<application_json_body>::value())) {
    assert(b.started && b.ended);
  }
  explicit http_request(const Url& url, bool keep_alive = true)
      : headers(generate_http_headers_get(url, keep_alive)) {
  }
};

}  // namespace tgbm
