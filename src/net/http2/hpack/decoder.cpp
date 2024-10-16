
#include <charconv>

#include "tgbm/net/http2/hpack/integers.hpp"
#include "tgbm/net/http2/hpack/decoder.hpp"

#include "tgbm/tools/scope_exit.h"

namespace tgbm::hpack {

[[nodiscard]] constexpr static size_t max_huffman_string_size_after_decode(
    size_type huffman_str_len) noexcept {
  // minimal symbol in table is 5 bit len, so worst case is only 5 bit symbols
  return size_t(huffman_str_len) * 8 / 5;
}

// uint16_t(-1) if not found
uint16_t huffman_decode_table_find(sym_info_t info);

// precondition: in != e
// note: 'len' must be decoded before calling this function
template <Out O>
static O decode_string_huffman(In in, size_type len, O out) {
  In e = in + len;
  sym_info_t info{0, 0};
  int bit_nmb = 0;
  auto next_bit = [&] {
    bool bit = *in & (0b1000'0000 >> bit_nmb);
    if (bit_nmb == 7) {
      bit_nmb = 0;
      ++in;
    } else {
      ++bit_nmb;
    }
    return bit;
  };
  for (;;) {
    // min symbol len in Huffman table is 5 bits
    for (int i = 0; in != e && i < 5; ++i, ++info.bit_count) {
      info.bits <<= 1;
      info.bits += next_bit();
    }
    uint16_t sym;
    while ((sym = huffman_decode_table_find(info)) == uint16_t(-1) && in != e) {
      info.bits <<= 1;
      info.bits += next_bit();
      ++info.bit_count;
    }
    if (sym == 256) [[unlikely]] {
      // EOS
      while (bit_nmb != 0)  // skip padding
        next_bit();
      return out;
    }
    if (sym != uint16_t(-1)) {
      info = {};
      *out = byte_t(sym);
      ++out;
    }
    if (in == e) {
      if (std::countr_one(info.bits) != info.bit_count)
        handle_protocol_error();  // incorrect padding
      return out;
    }
  }
  return out;
}

void decoded_string::set_huffman(const char* ptr, size_type len) {
  // also handles case when len == 0
  if (bytes_allocated() >= max_huffman_string_size_after_decode(len)) {
    const byte_t* in = (const byte_t*)ptr;
    // const cast because im owner of pointer (its allocated by malloc)
    char* end = decode_string_huffman(in, len, const_cast<char*>(data));
    sz = end - data;

    assert(sz <= max_huffman_string_size_after_decode(sz));
  } else {
    size_t sz_to_allocate = std::bit_ceil(max_huffman_string_size_after_decode(len));
    allocated_sz_log2 = std::bit_width(sz_to_allocate) - 1;
    const char* old_data = data;
    data = (char*)malloc(sz_to_allocate);

    on_scope_failure(free_mem) {
      free((void*)data);
      data = old_data;
      allocated_sz_log2 = 0;
    };
    // recursive call into branch where we have enough memory
    set_huffman(ptr, len);

    free_mem.no_longer_needed();
  }
}

// decodes partly indexed / new-name pairs
static void decode_header_impl(In& in, In e, uint8_t N, dynamic_table_t& dyntab, header_view& out) {
  index_type index = decode_integer(in, e, N);
  if (index == 0)
    decode_string(in, e, out.name);
  else
    out.name = get_by_index(index, &dyntab).name;
  decode_string(in, e, out.value);
}

static void decode_header_fully_indexed(In& in, In e, dynamic_table_t& dyntab, header_view& out) {
  assert(*in & 0b1000'0000);
  index_type index = decode_integer(in, e, 7);
  table_entry entry = get_by_index(index, &dyntab);
  // only way to get uncached value is from static table,
  // in dynamic table empty header value ("") is a cached header
  if (index < static_table_t::first_unused_index && entry.value.empty())
    handle_protocol_error();
  out = entry;
}

// header with incremental indexing
static void decode_header_cache(In& in, In e, dynamic_table_t& dyntab, header_view& out) {
  assert(in != e && *in & 0b0100'0000);
  decode_header_impl(in, e, 6, dyntab, out);
  dyntab.add_entry(out.name.str(), out.value.str());
}

static void decode_header_without_indexing(In& in, In e, dynamic_table_t& dyntab, header_view& out) {
  assert(in != e && (*in & 0x1111'0000) == 0);
  return decode_header_impl(in, e, 4, dyntab, out);
}

static void decode_header_never_indexing(In& in, In e, dynamic_table_t& dyntab, header_view& out) {
  assert(in != e && *in & 0b0001'0000);
  return decode_header_impl(in, e, 4, dyntab, out);
}

// returns requested new size of dynamic table
static size_type decode_dynamic_table_size_update(In& in, In e) noexcept {
  assert(*in & 0b0010'0000 && !(*in & 0b0100'0000) && !(*in & 0b1000'0000));
  return decode_integer(in, e, 5);
}

void decode_string(In& in, In e, decoded_string& out) {
  bool is_huffman = *in & 0b1000'0000;
  size_type str_len = decode_integer(in, e, 7);
  if (str_len > std::distance(in, e))
    handle_size_error();
  if (is_huffman)
    out.set_huffman((const char*)in, str_len);
  else
    out = std::string_view((const char*)in, str_len);
  in += str_len;
}

void decoder::decode_header(In& in, In e, header_view& out) {
  if (*in & 0b1000'0000)
    return decode_header_fully_indexed(in, e, dyntab, out);
  if (*in & 0b0100'0000)
    return decode_header_cache(in, e, dyntab, out);
  if (*in & 0b0010'0000) {
    dyntab.update_size(decode_dynamic_table_size_update(in, e));
    out.name.reset();
    out.value.reset();
    return;
  }
  if (*in & 0b0001'0000)
    return decode_header_never_indexing(in, e, dyntab, out);
  if ((*in & 0b1111'0000) == 0)
    return decode_header_without_indexing(in, e, dyntab, out);
  handle_protocol_error();
}

// returns status code
int decoder::decode_response_status(In& in, In e) {
  if (*in & 0b1000'0000) {
    // fast path, fully indexed
    auto in_before = in;
    index_type index = decode_integer(in, e, 7);
    switch (index) {
      case static_table_t::status_200:
        return 200;
      case static_table_t::status_204:
        return 204;
      case static_table_t::status_206:
        return 206;
      case static_table_t::status_304:
        return 304;
      case static_table_t::status_400:
        return 400;
      case static_table_t::status_404:
        return 404;
      case static_table_t::status_500:
        return 500;
    }
    in = in_before;
  }
  // first header of response must be required pseudoheader,
  // which is (for response) only one - ":status"
  header_view header;
  decode_header(in, e, header);
  std::string_view code = header.value.str();
  if (header.name.str() != ":status" || code.size() != 3)
    handle_protocol_error();
  int status_code;
  auto [_, err] = std::from_chars(code.data(), code.data() + 3, status_code);
  if (err != std::errc{})
    handle_protocol_error();
  return status_code;
}

}  // namespace tgbm::hpack
