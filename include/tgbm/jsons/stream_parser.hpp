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
  // Note: MUST NOT be reused after parsing value.
  // each value should be parsed by new stream_parser
  void feed(std::string_view data, bool end) {
    assert(!p.handler().is_done());
    io_error_code ec;
    try {
      p.write_some(!end, data.data(), data.size(), ec);
    } catch (std::exception& e) {
      throw parse_error(
          std::format("stream_parser fail, err: `{}`, is last part: {}, input: `{}`", e.what(), end, data));
    }
    if (ec) [[unlikely]] {
      throw parse_error(std::format("stream_parser fail, err: `{}`, is last part: {}, input: `{}`",
                                    ec.message(), end, data));
    }
    // The json output has ended, but the generator is still expecting more tokens
    if (end && !p.handler().is_done()) [[unlikely]]
      throw parse_error(std::format("input is over, but parsing not ended, input: `{}`", data));
  }
};

}  // namespace tgbm::json
