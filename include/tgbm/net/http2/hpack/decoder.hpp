#pragma once

#include "tgbm/tools/macro.hpp"

#include "tgbm/net/http2/hpack/basic_types.hpp"
#include "tgbm/net/http2/hpack/dynamic_table.hpp"

namespace tgbm::hpack {

struct TGBM_TRIVIAL_ABI decoded_string {
 private:
  const char* data = nullptr;
  size_type sz = 0;
  uint8_t allocated_sz_log2 = 0;  // != 0 after decoding huffman str

  friend void decode_string(In&, In, decoded_string&);

  void set_huffman(const char* ptr, size_type len);

 public:
  decoded_string() = default;

  decoded_string(const char* ptr, size_type len, bool is_huffman_encoded) : data(ptr), sz(len) {
    if (!is_huffman_encoded)
      return;
    set_huffman(ptr, len);
  }

  // precondition: str.size() less then max of size_type
  decoded_string(std::string_view str, bool is_huffman_encoded)
      : decoded_string(str.data(), str.size(), is_huffman_encoded) {
    assert(std::in_range<size_type>(str.size()));
  }

  decoded_string(decoded_string&& other) noexcept {
    swap(other);
  }

  decoded_string& operator=(decoded_string&& other) noexcept {
    swap(other);
    return *this;
  }

  // not huffman encoded string
  decoded_string& operator=(std::string_view str) noexcept {
    assert(std::in_range<size_type>(str.size()));
    reset();
    data = str.data();
    sz = str.size();
    return *this;
  }

  void swap(decoded_string& other) noexcept {
    std::swap(data, other.data);
    std::swap(sz, other.sz);
    std::swap(allocated_sz_log2, other.allocated_sz_log2);
  }

  friend void swap(decoded_string& l, decoded_string& r) noexcept {
    l.swap(r);
  }

  ~decoded_string() {
    reset();
  }

  void reset() noexcept {
    if (allocated_sz_log2)
      free((void*)data);
    data = nullptr;
    sz = 0;
    allocated_sz_log2 = 0;
  }

  [[nodiscard]] size_t bytes_allocated() const noexcept {
    if (!allocated_sz_log2)
      return 0;
    return 1 << allocated_sz_log2;
  }

  [[nodiscard]] std::string_view str() const noexcept {
    return std::string_view(data, sz);
  }

  // true if not empty
  explicit operator bool() const noexcept {
    return sz != 0;
  }

  bool operator==(const decoded_string& other) const noexcept {
    return str() == other.str();
  }
  bool operator==(std::string_view other) const noexcept {
    return str() == other;
  }

  std::strong_ordering operator<=>(const decoded_string& other) const noexcept {
    return str() <=> other.str();
  }

  std::strong_ordering operator<=>(std::string_view other) const noexcept {
    return str() <=> other;
  }
};

// note: decoding next header invalidates previous header
struct header_view {
  decoded_string name;
  decoded_string value;

  // header may be not present if default contructed or table_size_update happen instead of header
  explicit operator bool() const noexcept {
    return name || value;
  }

  header_view& operator=(header_view&&) = default;
  header_view& operator=(table_entry entry) {
    name = entry.name;
    value = entry.value;
    return *this;
  }
};

void decode_string(In& in, In e, decoded_string& out);

struct decoder {
  dynamic_table_t dyntab;

  // 4096 - default size in HTTP/2
  explicit decoder(size_type max_dyntab_size = 4096,
                   std::pmr::memory_resource* resource = std::pmr::get_default_resource())
      : dyntab(max_dyntab_size, resource) {
  }

  decoder(decoder&&) = default;
  decoder& operator=(decoder&&) noexcept = default;
  /*
   Note: this function ignores special 'cookie' header case
   https://www.rfc-editor.org/rfc/rfc7540#section-8.1.2.5
   and protocol error if decoded header name is not lowercase
  */
  void decode_header(In& in, In e, header_view& out);

  // returns status code
  int decode_response_status(In& in, In e);
};

}  // namespace tgbm::hpack
