#pragma once

#include <concepts>

#include "tgbm/net/http2/hpack/basic_types.hpp"

namespace tgbm::hpack {

// postcondition: do not overwrites highest 8 - N bits in *out first bytee
// precondition: low N bits of *out first byte is 0
template <std::unsigned_integral UInt = size_type, Out O>
O encode_integer(std::type_identity_t<UInt> I, uint8_t N, O _out) noexcept {
  auto out = noexport::adapt_output_iterator(_out);
  assert(N <= 8 && ((*out & ((1 << N) - 1)) == 0));
  /*
  pseudocode from RFC
   if I < 2^N - 1, encode I on N bits
   else
       encode (2^N - 1) on N bits
       I = I - (2^N - 1)
       while I >= 128
            encode (I % 128 + 128) on 8 bits
            I = I / 128
       encode I on 8 bits
  */
  const uint8_t prefix_max = (1 << N) - 1;
  assert((*out & prefix_max) == 0 && "precondition: low N bits of *out first byte is 0");
  auto push = [&out](uint8_t c) {
    *out = c;
    ++out;
  };
  if (I < prefix_max) {
    // write byte without overwriting existing first 8 - N bits
    *out |= uint8_t(I);
    ++out;
    return noexport::unadapt<O>(out);
  }
  // write byte without overwriting existing first 8 - N bits
  *out |= prefix_max;
  ++out;
  I -= prefix_max;
  while (I >= 128) {
    auto quot = I / 128;
    auto rem = I % 128;
    I = quot;
    push(rem | 0b1000'0000);
  }
  push(I);
  return noexport::unadapt<O>(out);
}

template <std::unsigned_integral UInt = size_type>
[[nodiscard]] size_type decode_integer(In& in, In e, uint8_t N) {
  const UInt prefix_mask = (1 << N) - 1;
  // get first N bits
  auto pull = [&] {
    if (in == e)
      handle_size_error();
    int8_t i = *in;
    ++in;
    return i;
  };
  UInt I = pull() & prefix_mask;
  if (I < prefix_mask)
    return I;
  uint8_t M = 0;
  uint8_t B;
  do {
    B = pull();
    UInt cpy = I;
    I += UInt(B & 0b0111'1111) << M;
    if (I < cpy)  // overflow
      handle_protocol_error();
    M += 7;
  } while (B & 0b1000'0000);
  return I;
}

}  // namespace tgbm::hpack
