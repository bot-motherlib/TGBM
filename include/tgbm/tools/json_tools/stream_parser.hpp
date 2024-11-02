#pragma once

#include "tgbm/logger.hpp"
#include "tgbm/tools/json_tools/boost_parse_generator.hpp"

namespace tgbm::json {
struct stream_parser {
  ::boost::json::basic_parser<boost::details::wait_handler> p;

  template <typename T>
  explicit stream_parser(T& t) : p(::boost::json::parse_options{}, t) {
  }

  void feed(std::string_view data, bool end) {
    ::boost::system::error_code ec;
    p.write_some(!end, data.data(), data.size(), ec);
    if (ec || (end && !p.handler().ended)) {
      TGBM_LOG_ERROR("ec: {}, ended: {}", ec.message(), p.handler().ended);
      TGBM_JSON_PARSE_ERROR;
    }
  }

  void feed(std::string_view data, bool end, ::boost::system::error_code& ec) {
    p.write_some(!end, data.data(), data.size(), ec);
    if (end && !p.handler().ended) {
      TGBM_LOG_ERROR("ec: {}, ended: {}", ec.message(), p.handler().ended);
      TGBM_JSON_PARSE_ERROR;
    }
  }
};

template <typename T>
void from_json(std::string_view json, T& out) {
  stream_parser p(out);
  p.feed(json, true);
}

template <typename T>
T from_json(std::string_view json) {
  T out;
  from_json(json, out);
  return out;
}
}  // namespace tgbm::json
