#pragma once

// LLVM pointer_union TODO comment

#include <algorithm>
#include <cassert>
#include <memory>
#include <type_traits>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <array>

#include "anyany/noexport/file_begin.hpp"

namespace tgbm {

consteval size_t log2_constexpr(size_t n) {
  return n == 1 ? 0 : log2_constexpr(n / 2) + 1;
}

template <typename T, typename... Ts>
constexpr size_t find_first =
    [] {
      size_t result = -1;
      size_t i = -1;
      // for guaranteed sequence
      (void)std::initializer_list<bool>{((++i, std::is_same_v<T, Ts>)&&result == -1 && (result = i))...};
      return result;
    }  // INVOKED HERE
();

// no support to void types and error index too much
template <size_t I, typename... Types>
using element_at_t = std::tuple_element_t<I, std::tuple<Types...>>;

template <typename... Ts>
consteval bool is_unique_types() {
  size_t is[] = {find_first<Ts, Ts...>..., size_t(-1) /* avoid empty array */};
  std::sort(std::begin(is), std::end(is));
  return std::end(is) == std::adjacent_find(std::begin(is), std::end(is));
}

template <typename T>
struct pointer_like_traits;

template <typename T>
struct pointer_like_traits<T*> {
  static_assert(!std::is_void_v<T>, "dont use with void* to avoid errors");

  static void* to_void_ptr(T* P) {
    return P;
  }
  static T* from_void_ptr(void* P) {
    return static_cast<T*>(P);
  }

  static constexpr int num_low_bits_available = log2_constexpr(alignof(T));
};

template <typename T>
struct pointer_traits {
  static_assert(std::is_pointer_v<T>);

  using type = pointer_like_traits<std::remove_cvref_t<std::remove_pointer_t<std::remove_const_t<T>>>*>;
};

template <typename T>
using pointer_traits_t = typename pointer_traits<T>::type;

namespace noexport {

template <typename Ptr>
struct punned_ptr {
 private:
  alignas(Ptr) std::array<unsigned char, sizeof(intptr_t)> data;

 public:
  static_assert(sizeof(Ptr) == sizeof(intptr_t));

  // Asserts that allow us to let the compiler implement the destructor and
  // copy/move constructors
  static_assert(std::is_trivially_destructible_v<Ptr>);
  static_assert(std::is_trivially_copy_constructible_v<Ptr>);
  static_assert(std::is_trivially_move_constructible_v<Ptr>);

  constexpr punned_ptr() = default;
  explicit constexpr punned_ptr(intptr_t i) {
    *this = i;
  }

  constexpr intptr_t as_int() const {
    return std::bit_cast<intptr_t>(data);
  }

  constexpr operator intptr_t() const {
    return as_int();
  }

  constexpr punned_ptr& operator=(intptr_t v) {
    std::memcpy(&data, &v, sizeof(data));
    return *this;
  }
};

}  // namespace noexport

// pointer_int_pair - This class implements a pair of a pointer and small
// integer.  It is designed to represent this in the space required by one
// pointer by bitmangling the integer into the low part of the pointer.  This
// can only be done for small integers: typically up to 3 bits, but it depends
// on the number of bits available according to pointer_like_traits for the
// type.
//
// Note that pointer_int_pair always puts the int_val part in the highest bits
// possible.  For example, pointer_int_pair<void*, 1, bool> will put the bit for
// the bool into bit #2, not bit #0, which allows the low two bits to be used
// for something else.  For example, this allows:
//   pointer_int_pair<pointer_int_pair<void*, 1, bool>, 1, bool>
// ... and the two bools will land in different bits.
template <unsigned IntBits, typename IntType, typename PtrTraits>
struct pointer_int_pair {
 private:
  noexport::punned_ptr<void*> value;

  static_assert(PtrTraits::num_low_bits_available >= 0);
  struct info {
    enum MaskAndShiftConstants : uintptr_t {
      // ptr_bitmask - The bits that come from the pointer.
      ptr_bitmask = ~(uintptr_t)(((intptr_t)1 << PtrTraits::num_low_bits_available) - 1),

      // int_shift - The number of low bits that we reserve for other uses, and
      // keep zero.
      int_shift = (uintptr_t)PtrTraits::num_low_bits_available - IntBits,

      // int_mask - This is the unshifted mask for valid bits of the int type.
      int_mask = (uintptr_t)(((intptr_t)1 << IntBits) - 1),

      // shifted_int_mask - This is the bits for the integer shifted in place.
      shifted_int_mask = (uintptr_t)(int_mask << int_shift)
    };

    static void* get_ptr(intptr_t value) {
      return PtrTraits::from_void_ptr(reinterpret_cast<void*>(value & ptr_bitmask));
    }

    static intptr_t get_int(intptr_t value) {
      return (value >> int_shift) & int_mask;
    }

    static intptr_t update_ptr(intptr_t OrigValue, void* Ptr) {
      intptr_t PtrWord = reinterpret_cast<intptr_t>(PtrTraits::to_void_ptr(Ptr));
      assert((PtrWord & ~ptr_bitmask) == 0 && "Pointer is not sufficiently aligned");
      // Preserve all low bits, just update the pointer.
      return PtrWord | (OrigValue & ~ptr_bitmask);
    }

    static intptr_t update_int(intptr_t OrigValue, intptr_t Int) {
      intptr_t IntWord = static_cast<intptr_t>(Int);
      assert((IntWord & ~int_mask) == 0 && "Integer too large for field");
      // Preserve all bits other than the ones we are updating.
      return (OrigValue & ~shifted_int_mask) | IntWord << int_shift;
    }
  };

 public:
  constexpr pointer_int_pair() = default;

  enum {
    max_int_val = PtrTraits::num_low_bits_available ? ((1 << PtrTraits::num_low_bits_available) - 1) : 0
  };
  static_assert(((1 << IntBits) - 1) <= max_int_val);

  pointer_int_pair(void* ptr_val, IntType int_val) {
    set_ptr_and_int(ptr_val, int_val);
  }

  explicit pointer_int_pair(void* ptr_val) {
    init_with_ptr(ptr_val);
  }

  void* get_ptr() const {
    return info::get_ptr(value);
  }

  IntType get_int() const {
    return (IntType)info::get_int(value);
  }

  void set_ptr(void* ptr_val) & {
    value = info::update_ptr(value, ptr_val);
  }

  void set_int(IntType int_val) & {
    value = info::update_int(value, static_cast<intptr_t>(int_val));
  }

  void init_with_ptr(void* ptr_val) & {
    value = info::update_ptr(0, ptr_val);
  }

  void set_ptr_and_int(void* ptr_val, IntType int_val) & {
    value = info::update_int(info::update_ptr(0, ptr_val), static_cast<intptr_t>(int_val));
  }

  void* get_raw_ptr() const {
    return reinterpret_cast<void*>(value.as_int());
  }
};

template <typename IntType, typename PtrTraits>
struct pointer_int_pair_fallback {
 private:
  void* ptr;
  IntType i;

 public:
  constexpr pointer_int_pair_fallback() = default;

  enum { max_int_val = std::numeric_limits<IntType>::max() };

  pointer_int_pair_fallback(void* ptr_val, IntType int_val) {
    set_ptr_and_int(ptr_val, int_val);
  }

  explicit pointer_int_pair_fallback(void* ptr_val) {
    init_with_ptr(ptr_val);
  }

  void* get_ptr() const {
    return ptr;
  }

  IntType get_int() const {
    return i;
  }

  void set_ptr(void* ptr_val) & {
    ptr = ptr_val;
  }

  void set_int(IntType int_val) & {
    i = int_val;
  }

  void init_with_ptr(void* ptr_val) & {
    ptr = ptr_val;
    i = 0;
  }

  void set_ptr_and_int(void* ptr_val, IntType int_val) & {
    ptr = ptr_val;
    i = int_val;
  }

  void* get_raw_ptr() const {
    return ptr;
  }
};

namespace noexport {

// Determine the number of bits required to store integers with values < n.
// This is ceil(log2(n)).
constexpr int bits_required(unsigned n) {
  return n > 1 ? 1 + bits_required((n + 1) / 2) : 0;
}

// Provide pointer_like_traits for void* that is used by pointer_union
// for the template arguments.
template <typename... Ptrs>
struct PointerUnionUIntTraits {
  static_assert((std::is_pointer_v<Ptrs> && ...));

  static void* to_void_ptr(void* P) {
    return P;
  }
  static void* from_void_ptr(void* P) {
    return P;
  }
  static constexpr int num_low_bits_available =
      std::min<int>({pointer_traits_t<Ptrs>::num_low_bits_available...});
};

// friend of pointer_union
struct data_getter {
  static auto& get_data(auto& x) noexcept {
    return x.data;
  }
};
template <typename CRTP, typename T, size_t Index>
struct pointer_union_member1 {
  pointer_union_member1() = default;
  pointer_union_member1(T* v) {
    // set ptr and index into ptr_int pair in the pointer_union
    data_getter::get_data(static_cast<CRTP&>(*this))
        .set_ptr_and_int(const_cast<void*>(static_cast<const void*>(v)), Index);
  }

  CRTP& operator=(T* v) {
    data_getter::get_data(static_cast<CRTP&>(*this))
        .set_ptr_and_int(const_cast<void*>(static_cast<const void*>(v)), Index);
    return static_cast<CRTP&>(*this);
  };
};
// TODO select here
template <typename... Types>
using ptrint_pair_for = std::conditional_t<
    (noexport::bits_required(sizeof...(Types)) <= PointerUnionUIntTraits<Types*...>::num_low_bits_available),
    pointer_int_pair<noexport::bits_required(sizeof...(Types)), int, PointerUnionUIntTraits<Types*...>>,
    pointer_int_pair_fallback<int, PointerUnionUIntTraits<Types*...>>>;

}  // namespace noexport

// A discriminated union of two or more pointer types, with the discriminator
// in the low bit of the pointer.
template <typename... Ts>
struct AA_MSVC_EBO pointer_union
    : noexport::pointer_union_member1<pointer_union<Ts...>, Ts, find_first<Ts, Ts...>>... {
 private:
  // give bases access to 'data' field
  friend noexport::data_getter;

  static_assert((std::is_same_v<std::decay_t<Ts>, Ts> && ...));
  static_assert(is_unique_types<Ts...>(), "pointer_union alternative types cannot be repeated");
  noexport::ptrint_pair_for<Ts...> data;

 public:
  pointer_union() = default;

  pointer_union(std::nullptr_t) : pointer_union() {
  }
  // Assignment from nullptr which just clears the union.
  const pointer_union& operator=(std::nullptr_t) {
    data.init_with_ptr(nullptr);
    return *this;
  }

  // constructors from all Ts*
  using noexport::pointer_union_member1<pointer_union<Ts...>, Ts,
                                        find_first<Ts, Ts...>>::pointer_union_member1...;
  // operator= from all Ts*
  using noexport::pointer_union_member1<pointer_union<Ts...>, Ts, find_first<Ts, Ts...>>::operator=...;

  explicit operator bool() const {
    return data.get_ptr() != nullptr;
  }
  int index() const noexcept {
    return data.get_int();
  }

  template <typename T>
  T* get_if() noexcept {
    using U = std::remove_const_t<T>;
    static_assert(find_first<U, Ts...> != -1);
    if (index() == find_first<U, Ts...>)
      return reinterpret_cast<T*>(data.get_ptr());
    return nullptr;
  }
  template <typename T>
  T* get_if() const noexcept {
    using U = std::remove_const_t<T>;
    static_assert(find_first<U, Ts...> != -1);
    if (index() == find_first<U, Ts...>)
      return reinterpret_cast<const T*>(data.get_ptr());
    return nullptr;
  }

  // -1 if no such type
  template <typename T>
  static consteval int indexof() {
    return find_first<T, Ts...>;
  }

  template <typename V>
  void visit(V&& visitor) {
    const int realindex = index();
    auto visit_one = [&]<typename T>(int index, std::type_identity<T>) {
      if (index == realindex) {
        visitor(get_if<std::remove_pointer_t<T>>());
        return false;
      }
      return true;
    };
    (visit_one(indexof<Ts>(), std::type_identity<Ts>{}) && ...);
  }
  template <typename V>
  void visit(V&& visitor) const {
    const int realindex = index();
    auto visit_one = [&]<typename T>(int index, std::type_identity<T>) {
      if (index == realindex) {
        visitor(get_if<std::remove_pointer_t<T>>());
        return false;
      }
      return true;
    };
    (visit_one(indexof<Ts>(), std::type_identity<Ts>{}) && ...);
  }

  void* get_raw_ptr() const {
    return data.get_raw_ptr();
  }
};

template <typename CRTP, typename T>
struct box_union_member1 {
  box_union_member1(const T& value) {
    static_cast<CRTP&>(*this).data = new T(value);
  }
  box_union_member1(T&& value) {
    static_cast<CRTP&>(*this).data = new T(std::move(value));
  }
  CRTP& operator=(const T& value) {
    CRTP& self = static_cast<CRTP&>(*this);
    return self = CRTP(std::in_place_type<T>(value));
  }
  CRTP& operator=(T&& value) {
    CRTP& self = static_cast<CRTP&>(*this);
    return self = CRTP(std::in_place_type<T>(std::move(value)));
  }
  // assumes 'ptr' may be released with 'delete'
  static CRTP from_raw_ptr(T* ptr) noexcept {
    CRTP s;
    s.data = ptr;
    return s;
  }
};

template <typename... Types>
struct [[clang::trivial_abi]] AA_MSVC_EBO box_union : box_union_member1<box_union<Types...>, Types>... {
 private:
  friend struct noexport::data_getter;

  static_assert((std::is_same_v<Types, std::decay_t<Types>> && ...));
  static_assert((!std::is_array_v<Types> && ...));  // TODO default_delete
  pointer_union<Types...> data = nullptr;

 public:
  using box_union_member1<box_union<Types...>, Types>::box_union_member1...;
  using box_union_member1<box_union<Types...>, Types>::operator=...;
  using box_union_member1<box_union<Types...>, Types>::from_raw_ptr...;

  box_union() = default;

  constexpr ~box_union() {
    reset();
  }

  constexpr box_union(const box_union& other) {
    if (!other.data)
      return;
    other.data.visit([&]<typename T>(T* ptr) { data = new T(*ptr); });
  }
  constexpr box_union(box_union&& other) noexcept : data(std::exchange(other.ptr, nullptr)) {
  }

  template <typename T, typename... Args>
  constexpr box_union(std::in_place_type_t<T>, Args&&... args) : data(new T(std::forward<Args>(args)...)) {
  }
  template <size_t I, typename... Args>
  constexpr box_union(std::in_place_index_t<I>, Args&&... args)
      : box_union<element_at_t<I, Types...>>(std::forward<Args>(args)...) {
  }
  template <typename T, typename... Args>
  constexpr T& emplace(Args&&... args) {
    static_assert(find_first<T, Types...> != -1);
    *this = box_union(std::in_place_type<T>, std::forward<Args>(args)...);
    return data.template get_if<T>();
  }
  template <size_t I, typename... Args>
  constexpr element_at_t<I, Types...>& emplace(Args&&... args) {
    return emplace<element_at_t<I, Types...>>(std::forward<Args>(args)...);
  }

  constexpr void reset() noexcept {
    static_assert((sizeof(Types) && ...));
    if (data) {
      data.visit([&]<typename T>(T* p) { delete p; });
      data = nullptr;
    }
  }

  constexpr void swap(box_union& other) noexcept {
    std::swap(data, other.data);
  }
  constexpr friend void swap(box_union& a, box_union& b) noexcept {
    a.swap(b);
  }
  constexpr box_union& operator=(const box_union& other) {
    *this = box_union(other);
    return *this;
  }
  constexpr box_union& operator=(box_union&& other) noexcept {
    swap(other);
    return *this;
  }

  constexpr explicit operator bool() const noexcept {
    return !!data;
  }
  template <typename V>
  constexpr void visit(V&& vtor) {
    return data.visit(std::forward<V>(vtor));
  }
  template <typename V>
  constexpr void visit(V&& vtor) const {
    return data.visit(std::forward<V>(vtor));
  }
};

}  // namespace tgbm

#include <anyany/noexport/file_end.hpp>
