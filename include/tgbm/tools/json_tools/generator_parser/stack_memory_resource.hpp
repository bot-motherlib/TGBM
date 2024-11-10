#pragma once

#include <span>

#include <kelcoro/memory_support.hpp>

#include <tgbm/tools/macro.hpp>
#include "tgbm/tools/memory.hpp"

namespace tgbm {

// assumes, that only last allocated chunk may be deallocated (like real stack)
struct stack_resource {
 private:
  std::span<byte_t> initial_buffer;
  byte_t* b = nullptr;
  byte_t* m = nullptr;
  byte_t* e = nullptr;
  TGBM_PIN;

  void drop_cur_chunk() {
    assert(b == m);
    assert(b != initial_buffer.data());
    byte_t* raw_chunk_begin = (byte_t*)b - sizeof(chunk_header);
    chunk_header* chunk = (chunk_header*)raw_chunk_begin;
    b = chunk->b;
    m = chunk->m;
    e = chunk->e;
    delete[] raw_chunk_begin;
  }

 public:
  // precondition: 'bytes' aligned correctly to dd::coroframe_align
  explicit stack_resource(std::span<byte_t> bytes)
      : initial_buffer(bytes), b(bytes.data()), m(b), e(b + bytes.size()) {
    assert(!bytes.empty());
    assert(((uintptr_t)bytes.data() % dd::coroframe_align()) == 0 && "incorrect align!");
  }

  ~stack_resource() {
    while (b != initial_buffer.data()) {
      m = b;
      drop_cur_chunk();
    }
  }
  std::size_t bytes_len(std::size_t bytes) {
    return bytes + dd::noexport::padding_len<dd::coroframe_align()>(bytes);
  }

  struct alignas(__STDCPP_DEFAULT_NEW_ALIGNMENT__) chunk_header {
    // prev state
    byte_t *b, *m, *e;
  };

  void* allocate(size_t len) {
    assert(len != 0);
    len = bytes_len(len);
    if (e - m >= len) {
      assert(((uintptr_t)(m + len) % 16) == 0);
      return std::exchange(m, m + len);
    }
    // allocate atleast * 2 current size
    const size_t alloc_bytes = (len + (e - b)) * 2;
    byte_t* chunk = new byte_t[sizeof(chunk_header) + alloc_bytes];
    new (chunk) chunk_header(b, m, e);
    b = chunk + sizeof(chunk_header);
    m = b + len;
    e = b + alloc_bytes;
    assert(((uintptr_t)b % 16) == 0);
    return b;
  }

  void deallocate(void* ptr, size_t len) noexcept {
    assert(((uintptr_t)ptr % 16) == 0);
    len = bytes_len(len);
    // not in current chunk, must be in some of prev
    // in most cases its literaly previous chunk, but its possible
    // if all memory from chunk deallocated + then allocated > chunk size
    // that prev chunk will be empty
    while ((byte_t*)ptr != m - len) {
      assert(ptr < b || ptr >= e);
      drop_cur_chunk();
    }
    m -= len;
  }
};

using with_stack_resource = dd::with_resource<dd::chunk_from<stack_resource>>;

}  // namespace tgbm
