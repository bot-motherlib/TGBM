#include "tgbm/net/http2/hpack.hpp"
#include "tgbm/net/http_base.hpp"
#include "tgbm/net/http2/protocol.hpp"

namespace tgbm::http2 {

void parse_http2_request_headers(hpack::decoder& d, std::span<const hpack::byte_t> bytes, http_request& req) {
  const auto* in = bytes.data();
  const auto* e = in + bytes.size();
  hpack::header_view header;

  // parse required pseudoheaders

  bool scheme_parsed = false;
  bool path_parsed = false;
  bool method_parsed = false;
  bool authority_parsed = false;
  while (in != e) {
    d.decode_header(in, e, header);
    if (!header)  // skip dynamic size updates
      continue;
    if (header.name == ":path") {
      if (path_parsed)
        throw protocol_error{};
      path_parsed = true;
      req.path = header.value.str();
    } else if (header.name == ":method") {
      if (method_parsed)
        throw protocol_error{};
      method_parsed = true;
      enum_from_string(header.value.str(), req.method);
    } else if (header.name == ":scheme") {
      if (scheme_parsed)
        throw protocol_error{};
      scheme_parsed = true;
      enum_from_string(header.value.str(), req.scheme);
    } else if (header.name == ":authority") {
      if (authority_parsed)
        throw protocol_error{};
      authority_parsed = true;
      req.authority = header.value.str();
    } else {
      goto push_header;
    }
  }
  while (in != e) {
    d.decode_header(in, e, header);
    if (!header)
      continue;
  push_header:
    req.headers.push_back(http_header_t(std::string(header.name.str()), std::string(header.value.str())));
  }
}

}  // namespace tgbm::http2
