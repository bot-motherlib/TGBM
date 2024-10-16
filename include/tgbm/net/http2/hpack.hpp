#pragma once

#include <span>

#include "tgbm/net/http2/hpack/encoder.hpp"
#include "tgbm/net/http2/hpack/decoder.hpp"

namespace tgbm::hpack {

// if 'Cache' is true, then tries to use dynamic table for indexing header
// and reduce size in next decoding
// if 'HUffman' is true, then strings will be encoded with huffman encoding
template <bool Cache = false, bool Huffman = false, Out O>
O encode_headers_block(encoder& enc, auto&& range_of_headers, O out) {
  for (auto&& [name, value] : range_of_headers)
    out = enc.template encode<Cache, Huffman>(name, value, out);
  return out;
}

// visitor should accept two string_views, name and value
// ignores (may be handled by caller side):
//  * special case Cookie header separated by key-value pairs
//  * possibly not lowercase headers (it should be protocol error,
//    but nothing breaks anyway, useless check)
template <typename V>
V decode_headers_block(decoder& dec, std::span<const byte_t> bytes, V visitor) {
  const auto* in = bytes.data();
  const auto* e = in + bytes.size();
  header_view header;
  while (in != e) {
    dec.decode_header(in, e, header);
    if (header)  // dynamic size update decoded without error
      visitor(header.name.str(), header.value.str());
  }
  return visitor;
}

}  // namespace tgbm::hpack
