#pragma once

#include <iterator>
#include <algorithm>
#include <ranges>

#include <kelcoro/operation_hash.hpp>

namespace tgbm {
template <std::input_iterator I1, std::sentinel_for<I1> S1, std::input_iterator I2, std::sentinel_for<I2> S2,
          typename Pred = std::ranges::equal_to, typename Proj1 = std::identity,
          typename Proj2 = std::identity>
  requires std::indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
constexpr bool starts_with(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = {}, Proj1 proj1 = {},
                           Proj2 proj2 = {}) {
  return std::ranges::mismatch(std::move(first1), last1, std::move(first2), last2, std::move(pred),
                               std::move(proj1), std::move(proj2))
             .in2 == last2;
}

template <std::ranges::input_range R1, std::ranges::input_range R2, typename Pred = std::ranges::equal_to,
          typename Proj1 = std::identity, typename Proj2 = std::identity>
  requires std::indirectly_comparable<std::ranges::iterator_t<R1>, std::ranges::iterator_t<R2>, Pred, Proj1,
                                      Proj2>
constexpr bool starts_with(R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) {
  return starts_with(std::ranges::begin(r1), std::ranges::end(r1), std::ranges::begin(r2),
                     std::ranges::end(r2), std::move(pred), std::move(proj1), std::move(proj2));
}

template <typename T>
KELCORO_PURE constexpr size_t fnv_hash(const T& value) noexcept {
  static_assert(std::has_unique_object_representations_v<T> && std::is_trivially_copyable_v<T>);
  size_t val = ::dd::noexport::fnv_offset_basis;
  const unsigned char* bytes = reinterpret_cast<const unsigned char*>(std::addressof(value));
  for (int i = 0; i < sizeof(T); ++i) {
    val ^= static_cast<size_t>(bytes[i]);
    val *= dd::noexport::fnv_prime;
  }
  return val;
}

KELCORO_PURE constexpr size_t fnv_hash_bytes(std::string_view bytes) noexcept {
  size_t val = ::dd::noexport::fnv_offset_basis;
  for (char b : bytes) {
    val ^= static_cast<size_t>(b);
    val *= dd::noexport::fnv_prime;
  }
  return val;
}

}  // namespace tgbm
