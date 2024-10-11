#pragma once

#include "tgbm/tools/memory.hpp"

#include <memory_resource>

namespace tgbm {

struct reusable_buffer {
 private:
  byte_t* bytes = nullptr;
  size_t sz = 0;
  std::pmr::memory_resource* resource = nullptr;

 public:
  explicit reusable_buffer(std::pmr::memory_resource* mr = std::pmr::new_delete_resource())
      : resource(mr ? mr : std::pmr::new_delete_resource()) {
  }

  ~reusable_buffer() {
    clear();
  }

  void reserve(size_t count) {
    (void)get_buffer(count);
  }

  void clear() noexcept {
    if (bytes) {
      resource->deallocate(bytes, sz);
      bytes = nullptr;
      sz = 0;
    }
  }

  std::span<byte_t> get_max_buffer() noexcept {
    if (!bytes)
      return {};
    return std::span(bytes, sz);
  }
  // value will be invalidated on next call
  // may return uninitialized memory
  std::span<byte_t> get_buffer(size_t count) {
    if (sz >= count)
      return std::span<byte_t>(bytes, count);
    if (bytes)
      clear();
    bytes = static_cast<byte_t*>(resource->allocate(count));
    sz = count;
    return std::span<byte_t>(bytes, count);
  }

  std::span<byte_t> get_atleast(size_t count) {
    if (sz >= count)
      return std::span<byte_t>(bytes, sz);
    return get_buffer(count);
  }
};

}  // namespace tgbm
