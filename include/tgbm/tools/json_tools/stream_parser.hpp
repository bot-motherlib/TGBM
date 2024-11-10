#pragma once

#include "boost/json/error.hpp"
#include "tgbm/tools/json_tools/boost_parse_generator.hpp"
#include "tgbm/tools/json_tools/exceptions.hpp"
#include "tgbm/tools/json_tools/generator_parser/all.hpp"

namespace tgbm::json {

template <dd::memory_resource R = dd::new_delete_resource>
struct stream_parser {
  ::boost::json::basic_parser<boost::details::wait_handler<R>> p;

  template <typename T>
  explicit stream_parser(T& t, R resource = R{}) : p(::boost::json::parse_options{}, t, std::move(resource)) {
  }

  void feed(std::string_view data, bool end) {
    ::boost::system::error_code ec;
    feed(data, end, ec);
    if (ec) [[unlikely]] {
      throw parse_error(ec.what());
    }
  }

  void feed(std::string_view data, bool end, ::boost::json::error_code& ec) {
    p.write_some(!end, data.data(), data.size(), ec);
    if (ec) [[unlikely]] {
      return;
    }
    if (end && !p.handler().ended) [[unlikely]] {
      ec = ::boost::json::error::extra_data;
    }
  }
};

template <typename T, dd::memory_resource R = dd::new_delete_resource>
void from_json(std::string_view json, T& out, R resource = R{}) {
  stream_parser p(out, std::move(resource));
  p.feed(json, true);
}

template <typename T, dd::memory_resource R = dd::new_delete_resource>
T from_json(std::string_view json, R resource = R{}) {
  T out;
  from_json(json, out, std::move(resource));
  return out;
}

}  // namespace tgbm::json
