#include "tgbm/net/http2/hpack.hpp"

#include <random>
#include <deque>

#define TEST(name) static void test_##name()
#define error_if(...)    \
  if (!!(__VA_ARGS__)) { \
    exit(__LINE__);      \
  }

static void test_number(uint32_t value_to_encode, uint8_t prefix_length, uint32_t expected_bytes_filled = 0) {
  tgbm::hpack::encoder enc;

  uint8_t chars[10] = {};
  uint8_t* encoded_end = enc.encode_integer(value_to_encode, prefix_length, chars);
  if (expected_bytes_filled != 0)
    error_if(encoded_end - chars != expected_bytes_filled);
  const uint8_t* in = chars;
  uint32_t x = enc.decode_integer(in, encoded_end, prefix_length);
  error_if(x != value_to_encode);
  error_if(in != encoded_end);  // decoded all what encoded
}

TEST(encode_decode_integers) {
  test_number(1337, 5, 3);
  test_number(10, 5, 1);
  test_number(31, 5, 2);
  test_number(32, 5, 2);
  test_number(127, 5, 2);
  test_number(128, 5, 2);
  test_number(255, 8, 2);
  test_number(256, 8, 2);
  test_number(16383, 5, 3);
  test_number(100000, 5, 4);
  test_number(1048576, 5, 4);
  test_number(0, 5, 1);
  test_number(1, 5, 1);
  test_number(std::numeric_limits<tgbm::hpack::size_type>::max(), 5, 6);
  tgbm::hpack::encoder enc;

  uint8_t chars[10] = {};
  uint8_t* encoded_end =
      enc.encode_integer<uint64_t>(uint64_t(std::numeric_limits<uint32_t>::max()) + 1, 6, chars);
  const uint8_t* in = chars;
  try {
    (void)enc.decode_integer<uint32_t>(in, encoded_end, 6);
  } catch (tgbm::protocol_error&) {
    // must throw overflow
    return;
  }
  error_if(true);
}

// vector, ordering matters!
using headers_t = std::vector<std::pair<std::string, std::string>>;
using bytes_t = std::vector<uint8_t>;

// encoder by ref, because HPACK it statefull (between requests/responses)
template <bool Huffman = false>
static void test_encode(tgbm::hpack::encoder<>& enc, tgbm::hpack::size_type expected_dyntab_size,
                        headers_t headers_to_encode, bytes_t expected_encoded_bytes,
                        headers_t expected_dyntab_content) {
  std::vector<uint8_t> bytes(expected_encoded_bytes.size(), ~0);
  auto* out = bytes.data();
  for (auto&& [name, value] : headers_to_encode)
    out = enc.template encode_header_memory_effective</*Cache=*/true, Huffman>(name, value, out);
  error_if(bytes != expected_encoded_bytes);
  error_if(enc.dyntab.current_size() != expected_dyntab_size);
  for (auto&& [name, value] : expected_dyntab_content) {
    auto res = enc.dyntab.find(name, value);
    error_if(!res);
  }
}

static void test_decode(tgbm::hpack::encoder<>& enc, tgbm::hpack::size_type expected_dyntab_size,
                        headers_t expected_decoded_headers, bytes_t bytes_to_decode,
                        headers_t expected_dyntab_content) {
  assert(!bytes_to_decode.empty());
  bytes_t decoded_name(1000, 0);
  bytes_t decoded_value(1000, 0);
  decltype(expected_decoded_headers) decoded;
  const uint8_t* in = bytes_to_decode.data();
  auto* e = in + bytes_to_decode.size();
  while (in != e) {
    auto [name_out, value_out] = enc.decode_header(in, e, decoded_name.data(), decoded_value.data());
    decoded.emplace_back(std::string_view((char*)decoded_name.data(), (char*)name_out),
                         std::string_view((char*)decoded_value.data(), (char*)value_out));
  }
  error_if(decoded != expected_decoded_headers);
  error_if(enc.dyntab.current_size() != expected_dyntab_size);
  error_if(in - 1 != &bytes_to_decode.back());
  size_t i = tgbm::hpack::static_table_t::first_unused_index;
  size_t imax = i + expected_dyntab_content.size();
  for (auto&& [name, value] : expected_dyntab_content) {
    error_if(!enc.dyntab.find(name, value));
    ++i;
  }
}

// https://www.rfc-editor.org/rfc/rfc7541#appendix-C.3.1
TEST(encode_decode1) {
  tgbm::hpack::encoder sender(164);
  tgbm::hpack::encoder receiver(164);
  // first request
  {
    headers_t headers{
        {":method", "GET"},
        {":scheme", "http"},
        {":path", "/"},
        {":authority", "www.example.com"},
    };
    headers_t cached_headers{
        {":authority", "www.example.com"},
    };
    bytes_t bytes_expected{
        0x82, 0x86, 0x84, 0x41, 0x0f, 0x77, 0x77, 0x77, 0x2e, 0x65,
        0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d,
    };

    test_encode(sender, 57, headers, bytes_expected, cached_headers);

    test_decode(receiver, 57, headers, bytes_expected, cached_headers);
  }
  // second request
  {
    headers_t headers{
        {":method", "GET"},
        {":scheme", "http"},
        {":path", "/"},
        {":authority", "www.example.com"},
        {"cache-control", "no-cache"},
    };
    bytes_t bytes_expected{
        0x82, 0x86, 0x84, 0xbe, 0x58, 0x08, 0x6e, 0x6f, 0x2d, 0x63, 0x61, 0x63, 0x68, 0x65,
    };
    headers_t cached_headers{
        {"cache-control", "no-cache"},
        {":authority", "www.example.com"},
    };
    test_encode(sender, 110, headers, bytes_expected, cached_headers);
    test_decode(receiver, 110, headers, bytes_expected, cached_headers);
  }
  // third request
  {
    headers_t headers{
        {":method", "GET"},
        {":scheme", "https"},
        {":path", "/index.html"},
        {":authority", "www.example.com"},
        {"custom-key", "custom-value"},
    };
    bytes_t bytes_expected{0x82, 0x87, 0x85, 0xbf, 0x40, 0x0a, 0x63, 0x75, 0x73, 0x74,
                           0x6f, 0x6d, 0x2d, 0x6b, 0x65, 0x79, 0x0c, 0x63, 0x75, 0x73,
                           0x74, 0x6f, 0x6d, 0x2d, 0x76, 0x61, 0x6c, 0x75, 0x65};
    headers_t cached_headers{
        {"custom-key", "custom-value"},
        {"cache-control", "no-cache"},
        {":authority", "www.example.com"},
    };
    test_encode(sender, 164, headers, bytes_expected, cached_headers);
    test_decode(receiver, 164, headers, bytes_expected, cached_headers);
  }
}

TEST(encode_decode_huffman1) {
  tgbm::hpack::encoder sender(164);
  tgbm::hpack::encoder receiver(164);
  // first request
  {
    headers_t headers{
        {":method", "GET"},
        {":scheme", "http"},
        {":path", "/"},
        {":authority", "www.example.com"},
    };
    headers_t cached_headers{
        {":authority", "www.example.com"},
    };
    bytes_t bytes_expected{
        0x82, 0x86, 0x84, 0x41, 0x8c, 0xf1, 0xe3, 0xc2, 0xe5, 0xf2, 0x3a, 0x6b, 0xa0, 0xab, 0x90, 0xf4, 0xff,
    };

    test_encode<true>(sender, 57, headers, bytes_expected, cached_headers);

    test_decode(receiver, 57, headers, bytes_expected, cached_headers);
  }
  // second request
  {
    headers_t headers{
        {":method", "GET"},
        {":scheme", "http"},
        {":path", "/"},
        {":authority", "www.example.com"},
        {"cache-control", "no-cache"},
    };
    bytes_t bytes_expected{
        0x82, 0x86, 0x84, 0xbe, 0x58, 0x86, 0xa8, 0xeb, 0x10, 0x64, 0x9c, 0xbf,
    };
    headers_t cached_headers{
        {"cache-control", "no-cache"},
        {":authority", "www.example.com"},
    };
    test_encode<true>(sender, 110, headers, bytes_expected, cached_headers);
    test_decode(receiver, 110, headers, bytes_expected, cached_headers);
  }
  // third request
  {
    headers_t headers{
        {":method", "GET"},
        {":scheme", "https"},
        {":path", "/index.html"},
        {":authority", "www.example.com"},
        {"custom-key", "custom-value"},
    };
    bytes_t bytes_expected{
        0x82, 0x87, 0x85, 0xbf, 0x40, 0x88, 0x25, 0xa8, 0x49, 0xe9, 0x5b, 0xa9,
        0x7d, 0x7f, 0x89, 0x25, 0xa8, 0x49, 0xe9, 0x5b, 0xb8, 0xe8, 0xb4, 0xbf,
    };
    headers_t cached_headers{
        {"custom-key", "custom-value"},
        {"cache-control", "no-cache"},
        {":authority", "www.example.com"},
    };
    test_encode<true>(sender, 164, headers, bytes_expected, cached_headers);
    test_decode(receiver, 164, headers, bytes_expected, cached_headers);
  }
}

// similar to first example, but forces eviction of dynamic table entries
TEST(encode_decode_with_eviction) {
  tgbm::hpack::encoder sender(256);
  tgbm::hpack::encoder receiver(256);
  // first response
  {
    headers_t headers{
        {":status", "302"},
        {"cache-control", "private"},
        {"date", "Mon, 21 Oct 2013 20:13:21 GMT"},
        {"location", "https://www.example.com"},
    };
    headers_t cached_headers{
        {"location", "https://www.example.com"},
        {"date", "Mon, 21 Oct 2013 20:13:21 GMT"},
        {"cache-control", "private"},
        {":status", "302"},
    };
    bytes_t bytes_expected = {
        0x48, 0x03, 0x33, 0x30, 0x32, 0x58, 0x07, 0x70, 0x72, 0x69, 0x76, 0x61, 0x74, 0x65,
        0x61, 0x1d, 0x4d, 0x6f, 0x6e, 0x2c, 0x20, 0x32, 0x31, 0x20, 0x4f, 0x63, 0x74, 0x20,
        0x32, 0x30, 0x31, 0x33, 0x20, 0x32, 0x30, 0x3a, 0x31, 0x33, 0x3a, 0x32, 0x31, 0x20,
        0x47, 0x4d, 0x54, 0x6e, 0x17, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3a, 0x2f, 0x2f, 0x77,
        0x77, 0x77, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d,
    };

    test_encode(sender, 222, headers, bytes_expected, cached_headers);

    test_decode(receiver, 222, headers, bytes_expected, cached_headers);
  }
  // second response
  {
    headers_t headers{
        {":status", "307"},
        {"cache-control", "private"},
        {"date", "Mon, 21 Oct 2013 20:13:21 GMT"},
        {"location", "https://www.example.com"},
    };
    headers_t cached_headers{
        {":status", "307"},
        {"location", "https://www.example.com"},
        {"date", "Mon, 21 Oct 2013 20:13:21 GMT"},
        {"cache-control", "private"},
    };
    bytes_t bytes_expected = {0x48, 0x03, 0x33, 0x30, 0x37, 0xc1, 0xc0, 0xbf};

    test_encode(sender, 222, headers, bytes_expected, cached_headers);

    test_decode(receiver, 222, headers, bytes_expected, cached_headers);
  }
  // third response
  {
    headers_t headers{
        {":status", "200"},
        {"cache-control", "private"},
        {"date", "Mon, 21 Oct 2013 20:13:22 GMT"},
        {"location", "https://www.example.com"},
        {"content-encoding", "gzip"},
        {"set-cookie", "foo=ASDJKHQKBZXOQWEOPIUAXQWEOIU; max-age=3600; version=1"},
    };
    headers_t cached_headers{
        {"set-cookie", "foo=ASDJKHQKBZXOQWEOPIUAXQWEOIU; max-age=3600; version=1"},
        {"content-encoding", "gzip"},
        {"date", "Mon, 21 Oct 2013 20:13:22 GMT"},
    };
    bytes_t bytes_expected = {
        0x88, 0xc1, 0x61, 0x1d, 0x4d, 0x6f, 0x6e, 0x2c, 0x20, 0x32, 0x31, 0x20, 0x4f, 0x63, 0x74, 0x20, 0x32,
        0x30, 0x31, 0x33, 0x20, 0x32, 0x30, 0x3a, 0x31, 0x33, 0x3a, 0x32, 0x32, 0x20, 0x47, 0x4d, 0x54, 0xc0,
        0x5a, 0x04, 0x67, 0x7a, 0x69, 0x70, 0x77, 0x38, 0x66, 0x6f, 0x6f, 0x3d, 0x41, 0x53, 0x44, 0x4a, 0x4b,
        0x48, 0x51, 0x4b, 0x42, 0x5a, 0x58, 0x4f, 0x51, 0x57, 0x45, 0x4f, 0x50, 0x49, 0x55, 0x41, 0x58, 0x51,
        0x57, 0x45, 0x4f, 0x49, 0x55, 0x3b, 0x20, 0x6d, 0x61, 0x78, 0x2d, 0x61, 0x67, 0x65, 0x3d, 0x33, 0x36,
        0x30, 0x30, 0x3b, 0x20, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x3d, 0x31};

    test_encode(sender, 215, headers, bytes_expected, cached_headers);

    test_decode(receiver, 215, headers, bytes_expected, cached_headers);
  }
}

TEST(huffman) {
  std::string_view str = "hello world";
  uint8_t buf[20];
  auto* encoded_end = tgbm::hpack::encoder{}.encode_string<true>(str, +buf);
  const uint8_t* in = +buf;
  uint8_t outbuf[20];
  auto out2 = tgbm::hpack::encoder{}.decode_string(in, encoded_end, +outbuf);
  error_if(std::distance(outbuf, out2) != str.size());
  std::string decoded;
  for (uint8_t i : std::ranges::subrange(+outbuf, out2))
    decoded.push_back((char)i);
  error_if(decoded != str);
}

TEST(huffman_rand) {
  std::mt19937 gen(155);
  bytes_t bytes;
  for (int i = 0; i < 1000; ++i)
    bytes.push_back(std::uniform_int_distribution<int>(0, 255)(gen));
  tgbm::hpack::encoder e;
  bytes_t encoded;
  e.encode_string_huffman(std::string_view((char*)bytes.data(), (char*)bytes.data() + bytes.size()),
                          std::back_inserter(encoded));
  const uint8_t* in = encoded.data();
  bytes_t decoded;
  e.decode_string_huffman(in, in + encoded.size(), std::back_inserter(decoded));
  error_if(in != encoded.data() + encoded.size());
  error_if(decoded != bytes);
}

TEST(huffman_table_itself){
#define HUFFMAN_TABLE(index, bits, bitcount)                                                        \
  {                                                                                                 \
    error_if(tgbm::hpack::huffman_decode_table_find(tgbm::hpack::sym_info_t(0b##bits, bitcount)) != \
             uint16_t(index));                                                                      \
  }
#include "tgbm/net/http2/huffman_table.def"
}

TEST(huffman_encode_eos) {
  // encoded string ("!") and EOS
  bytes_t bytes{
      0x85, 0xfe, 0x3f, 0xff, 0xff, 0xff,
  };
  tgbm::hpack::encoder enc;
  const uint8_t* in = bytes.data();
  bytes_t decoded;
  enc.decode_string_huffman(in, in + bytes.size(), std::back_inserter(decoded));
  error_if(in != bytes.data() + bytes.size());
  error_if(std::string_view((char*)decoded.data(), decoded.size()) != "!");
}

TEST(static_table_find) {
  using st = tgbm::hpack::static_table_t;
  using namespace tgbm::hpack;
#define STATIC_TABLE_ENTRY(cppname, header_name, ...)                       \
  {                                                                         \
    auto res = st::find(header_name, "" __VA_OPT__(__VA_ARGS__));           \
    error_if(res.header_name_index != (index_type)static_table_t::cppname); \
    error_if(res.value_indexed != (0 __VA_OPT__(+1)));                      \
  }
#include "tgbm/net/http2/static_table.def"
}

struct test_dyntab_t {
  std::deque<std::pair<std::string, std::string>> d;
  size_t cur_size;
  size_t max_size;

  test_dyntab_t(size_t max_sz) : cur_size(0), max_size(max_sz) {
  }
  void add_entry(std::string name, std::string value) {
    size_t entry_sz = name.size() + value.size() + 32;  // RFC
    while (!d.empty() && entry_sz > max_size - cur_size) {
      cur_size -= (d.back().first.size() + d.back().second.size() + 32);
      d.pop_back();
    }
    if (entry_sz > max_size)
      return;
    d.push_front({std::move(name), std::move(value)});
    cur_size += entry_sz;
  }
};

static int64_t rand_int(int64_t min, int64_t max, std::mt19937& gen) {
  return std::uniform_int_distribution(min, max)(gen);
}
static std::string generate_random_string(size_t length, std::mt19937& gen) {
  constexpr std::string_view characters = "abcdefghijklmnopqrstuvwxyz";
  std::string random_str;
  random_str.reserve(length);
  for (size_t i = 0; i < length; ++i)
    random_str += characters[rand_int(0, characters.size() - 1, gen)];
  return random_str;
}

TEST(dynamic_table_indexes) {
  enum { MAX_SZ = 512 };
  tgbm::hpack::dynamic_table_t table(MAX_SZ);

  error_if(table.current_size() != 0);
  table.add_entry("name1", "hello world");
  table.add_entry("name2", "header2");
  table.add_entry(std::string(1000, 'a'), "");
  error_if(table.current_size() != 0);
  std::mt19937 gen(213214);

  test_dyntab_t test_table(MAX_SZ);

  for (int i = 0; i < 1000; ++i) {
    std::string random_name = generate_random_string(rand_int(1, 300, gen), gen);
    std::string random_value = generate_random_string(rand_int(0, 300, gen), gen);
    table.add_entry(random_name, random_value);
    auto r = table.find(random_name, random_value);
    if (random_name.size() + random_value.size() + 32 <= MAX_SZ) {
      // firstly inserted value always have first index
      error_if(r.header_name_index != 62);
      error_if(!r.value_indexed);
    } else {
      error_if(r.header_name_index != 0);
      error_if(r.value_indexed);
    }
    test_table.add_entry(random_name, random_value);
    error_if(table.current_size() != test_table.cur_size);
    // 62 is min index of HPACK dynamic table
    for (size_t j = 62; j < 62 + test_table.d.size(); ++j) {
      const auto& real_entry = table.get_entry(j);
      const auto& test_entry = test_table.d[j - 62];
      error_if(real_entry.name != test_entry.first);
      error_if(real_entry.value != test_entry.second);
    }
  }
}

TEST(tg_answer) {
  std::vector<tgbm::hpack::byte_t> bytes = {
      0x88, 0x76, 0x89, 0xaa, 0x63, 0x55, 0xe5, 0x80, 0xae, 0x17, 0x97, 0x7,  0x61, 0x96, 0xc3, 0x61, 0xbe,
      0x94, 0x3,  0x8a, 0x6e, 0x2d, 0x6a, 0x8,  0x2,  0x69, 0x40, 0x3b, 0x70, 0xf,  0x5c, 0x13, 0x4a, 0x62,
      0xd1, 0xbf, 0x5f, 0x8b, 0x1d, 0x75, 0xd0, 0x62, 0xd,  0x26, 0x3d, 0x4c, 0x74, 0x41, 0xea, 0x5c, 0x4,
      0x31, 0x39, 0x32, 0x36, 0x0,  0x91, 0x42, 0x6c, 0x31, 0x12, 0xb2, 0x6c, 0x1d, 0x48, 0xac, 0xf6, 0x25,
      0x64, 0x14, 0x96, 0xd8, 0x64, 0xfa, 0xa0, 0xa4, 0x7e, 0x56, 0x1c, 0xc5, 0x81, 0x90, 0xb6, 0xcb, 0x80,
      0x0,  0x3e, 0xd4, 0x35, 0x44, 0xa2, 0xd9, 0xb,  0xba, 0xd8, 0xef, 0x9e, 0x91, 0x9a, 0xa4, 0x7d, 0xa9,
      0x5d, 0x85, 0xa0, 0xe3, 0x93, 0x0,  0x93, 0x19, 0x8,  0x54, 0x21, 0x62, 0x1e, 0xa4, 0xd8, 0x7a, 0x16,
      0x1d, 0x14, 0x1f, 0xc2, 0xc7, 0xb0, 0xd3, 0x1a, 0xaf, 0x1,  0x2a, 0x0,  0x94, 0x19, 0x8,  0x54, 0x21,
      0x62, 0x1e, 0xa4, 0xd8, 0x7a, 0x16, 0x1d, 0x14, 0x1f, 0xc2, 0xd4, 0x95, 0x33, 0x9e, 0x44, 0x7f, 0x90,
      0xc5, 0x83, 0x7f, 0xd2, 0x9a, 0xf5, 0x6e, 0xdf, 0xf4, 0xa6, 0xad, 0x7b, 0xf2, 0x6a, 0xd3, 0xbb, 0x0,
      0x94, 0x19, 0x8,  0x54, 0x21, 0x62, 0x1e, 0xa4, 0xd8, 0x7a, 0x16, 0x2f, 0x9a, 0xce, 0x82, 0xad, 0x39,
      0x47, 0x21, 0x6c, 0x47, 0xa5, 0xbc, 0x7a, 0x92, 0x5a, 0x92, 0xb6, 0x72, 0xd5, 0x32, 0x67, 0xfa, 0xbc,
      0x7a, 0x92, 0x5a, 0x92, 0xb6, 0xff, 0x55, 0x97, 0xea, 0xf8, 0xd2, 0x5f, 0xad, 0xc5, 0xb3, 0xb9, 0x6c,
      0xfa, 0xbc, 0x7a, 0xaa, 0x29, 0x12, 0x63, 0xd5,
  };
  tgbm::hpack::decoder<> e;
  headers_t expected{
      {":status", "200"},
      {"server", "nginx/1.18.0"},
      {"date", "Fri, 06 Sep 2024 07:08:24 GMT"},
      {"content-type", "application/json"},
      {"content-length", "1926"},
      {"strict-transport-security", "max-age=31536000; includeSubDomains; preload"},
      {"access-control-allow-origin", "*"},
      {"access-control-allow-methods", "GET, POST, OPTIONS"},
      {"access-control-expose-headers", "Content-Length,Content-Type,Date,Server,Connection"},
  };
  headers_t result;
  const tgbm::hpack::byte_t* in = bytes.data();
  error_if(200 != e.decode_response_status(in, bytes.data() + bytes.size()));
  tgbm::hpack::decode_headers_block(e, bytes, [&](std::string_view name, std::string_view value) {
    result.push_back(std::pair{std::string(name), std::string(value)});
  });
  error_if(result != expected);
}

TEST(decode_status) {
  tgbm::hpack::decoder<> e;
  bytes_t rsp;

  e.encode_header_fully_indexed(tgbm::hpack::static_table_t::status_304, std::back_inserter(rsp));
  const auto* in = rsp.data();
  error_if(304 != e.decode_response_status(in, rsp.data() + rsp.size()));
  error_if(in != rsp.data() + rsp.size());
  rsp.clear();

  // use status as name and valid status code
  e.encode_header_without_indexing(tgbm::hpack::static_table_t::status_200, "200", std::back_inserter(rsp));
  in = rsp.data();
  error_if(200 != e.decode_response_status(in, rsp.data() + rsp.size()));
  error_if(in != rsp.data() + rsp.size());
  rsp.clear();

  e.encode_header_without_indexing(tgbm::hpack::static_table_t::status_200, "fds", std::back_inserter(rsp));
  in = rsp.data();
  try {
    e.decode_response_status(in, rsp.data() + rsp.size());
    error_if(true);
  } catch (...) {
  }
  rsp.clear();

  e.encode_header_without_indexing(tgbm::hpack::static_table_t::status_200, "2000", std::back_inserter(rsp));
  in = rsp.data();
  try {
    e.decode_response_status(in, rsp.data() + rsp.size());
    error_if(true);
  } catch (...) {
  }
  rsp.clear();

  e.encode_header_never_indexing(tgbm::hpack::static_table_t::status_200, "2 0 0", std::back_inserter(rsp));
  in = rsp.data();
  try {
    e.decode_response_status(in, rsp.data() + rsp.size());
    error_if(true);
  } catch (...) {
  }
  rsp.clear();

  e.encode_header_and_cache(tgbm::hpack::static_table_t::status_200, "555", std::back_inserter(rsp));
  in = rsp.data();
  error_if(555 != e.decode_response_status(in, rsp.data() + rsp.size()));
  error_if(in != rsp.data() + rsp.size());
  rsp.clear();
}

TEST(dynamic_table_size_update) {
  tgbm::hpack::decoder<> e;
  bytes_t bytes;

  e.encode_dynamic_table_size_update(144, std::back_inserter(bytes));
  const auto* in = bytes.data();
  const auto* end = bytes.data() + bytes.size();
  error_if(144 != e.decode_dynamic_table_size_update(in, end));
  error_if(in != end);  // all parsed
}

TEST(static_table_find_by_index) {
  using namespace tgbm::hpack;
  {
    find_result_t res;
    res = static_table_t::find(0, "");
    error_if(res.value_indexed || res.header_name_index);
    res = static_table_t::find(static_table_t::first_unused_index, "abc");
    error_if(res.value_indexed || res.header_name_index);
  }
  std::string possible_values[]{
#define STATIC_TABLE_ENTRY(cppname, header_name, ...) __VA_OPT__(__VA_ARGS__, )
#include "tgbm/net/http2/static_table.def"
  };
  std::string impossible_values[]{
      "",
      "fdsgwrg",
      "hello world",
  };
  auto test_index = [&](index_type i) {
    for (std::string_view val : possible_values) {
      auto myentry = static_table_t::get_entry(i);
      find_result_t res = static_table_t::find(i, val);
      find_result_t res2 = static_table_t::find(myentry.name, val);
      error_if(res.value_indexed != res2.value_indexed);
      error_if(static_table_t::get_entry(res.header_name_index).name != myentry.name);
      error_if(static_table_t::get_entry(res.header_name_index).name !=
               static_table_t::get_entry(res2.header_name_index).name);
      if (val == myentry.value)
        error_if(res.header_name_index != i);
    }
    for (std::string_view val : impossible_values) {
      auto res = static_table_t::find(i, val);
      error_if(res.value_indexed);
      error_if(res.header_name_index != i);
    }
  };
  for (index_type i = 1; i < static_table_t::first_unused_index; ++i)
    test_index(i);
  {
    auto res1 = static_table_t::find(static_table_t::path, "/");
    auto res2 = static_table_t::find(static_table_t::path_index_html, "/");
    error_if(res1.value_indexed != res2.value_indexed);
    error_if(res1.header_name_index != res2.header_name_index);
    error_if(res1.header_name_index != static_table_t::path);
  }
}

int main() {
  test_tg_answer();
  test_encode_decode_with_eviction();
  test_encode_decode_huffman1();
  test_encode_decode_integers();
  test_encode_decode1();
  test_huffman_table_itself();
  test_huffman();
  test_huffman_rand();
  test_huffman_encode_eos();
  test_static_table_find();
  test_dynamic_table_indexes();
  test_decode_status();
  test_dynamic_table_size_update();
  test_static_table_find_by_index();
}
