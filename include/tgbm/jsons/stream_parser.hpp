#pragma once

#include <boost/json/error.hpp>

#include <tgbm/jsons/boostjson_sax_producer.hpp>
#include <tgbm/jsons/errors.hpp>
#include <tgbm/jsons/sax_parser.hpp>
#include <tgbm/utils/memory.hpp>

namespace tgbm::json {

struct stream_parser {
  ::boost::json::basic_parser<boostjson_sax_producer> p;

  template <typename T>
  explicit stream_parser(T& t, std::span<byte_t> buf = {}) : p(::boost::json::parse_options{}, t, buf) {
  }

  void feed(std::string_view data, bool end) {
    ::boost::system::error_code ec;
    feed(data, end, ec);
    if (ec) [[unlikely]]
      throw parse_error(ec.what());
  }

  void feed(std::string_view data, bool end, io_error_code& ec) {
    p.write_some(!end, data.data(), data.size(), ec);
    if (ec) [[unlikely]]
      return;
    // The json output has ended, but the generator is still expecting more tokens
    if (end && !p.handler().is_done()) [[unlikely]]
      ec = ::boost::json::error::extra_data;
  }
};

}  // namespace tgbm::json
