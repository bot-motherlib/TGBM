#pragma once

#include <algorithm>

#include "tgbm/net/http2/hpack/basic_types.hpp"
#include "tgbm/net/http2/hpack/integers.hpp"

namespace tgbm::hpack {

template <Out O>
O encode_string_huffman(std::string_view str, O _out) {
  auto out = noexport::adapt_output_iterator(_out);
  // precalculate size
  // (size should be before string and len in bits depends on 'len' value)
  size_type len_after_encode = 0;
  for (char c : str)
    len_after_encode += huffman_table[uint8_t(c)].bit_count;
  *out = 0b1000'0000;  // set H bit
  const int padlen = 8 - len_after_encode % 8;
  out = encode_integer((len_after_encode + padlen) / 8, 7, out);
  auto push_bit = [&, bitn = 7](bool bit) mutable {
    if (bitn == 7)
      *out = 0;
    *out |= (bit << bitn);
    if (bitn == 0) {
      ++out;
      bitn = 7;
      // not set out to 0, because may be end
    } else {
      --bitn;
    }
  };
  for (char c : str) {
    sym_info_t bits = huffman_table[uint8_t(c)];
    for (int i = 0; i < bits.bit_count; ++i)
      push_bit(bits.bits & (1 << i));
  }
  // padding MUST BE formed from EOS la-la-la (just 111..)
  for (int i = 0; i < padlen; ++i)
    push_bit(true);
  return noexport::unadapt<O>(out);
}

template <bool Huffman = false, Out O>
O encode_string(std::string_view str, O _out) {
  auto out = noexport::adapt_output_iterator(_out);
  /*
       0   1   2   3   4   5   6   7
     +---+---+---+---+---+---+---+---+
     | H |    String Length (7+)     |
     +---+---------------------------+
     |  String Data (Length octets)  |
     +-------------------------------+
  */
  if constexpr (!Huffman) {
    *out = 0;  // set H bit to 0
    out = encode_integer(str.size(), 7, out);
    out = std::copy_n(str.data(), str.size(), out);
  } else {
    out = encode_string_huffman(str, out);
  }
  return noexport::unadapt<O>(out);
}

// in decoder.hpp
struct decoded_string;

void decode_string(In& in, In e, decoded_string& out);

}  // namespace tgbm::hpack
