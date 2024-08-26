#pragma once

// LLVM pointer_union (completely rewrited)
// and used to create box_union (similar to variant<box<Types>...> but sizeof == sizeof(void*))

#include <cassert>
#include <type_traits>
#include <cstdint>
#include <cstring>
#include <cstddef>

#include "anyany/noexport/file_begin.hpp"

#include "tgbm/tools/meta.hpp"
#include "tgbm/tools/box.h"

namespace tgbm {

namespace noexport {

template <typename Ptr>
struct punned_ptr {
 private:
  alignas(intptr_t) unsigned char data[sizeof(intptr_t)];

 public:
  static_assert(sizeof(Ptr) == sizeof(intptr_t));

  static_assert(std::is_trivially_destructible_v<Ptr>);
  static_assert(std::is_trivially_copy_constructible_v<Ptr>);
  static_assert(std::is_trivially_move_constructible_v<Ptr>);

  constexpr punned_ptr() = default;
  explicit constexpr punned_ptr(intptr_t i) noexcept {
    *this = i;
  }

  constexpr intptr_t as_int() const noexcept {
    return std::bit_cast<intptr_t>(data);
  }

  constexpr operator intptr_t() const noexcept {
    return as_int();
  }

  constexpr punned_ptr& operator=(intptr_t v) noexcept {
    std::memcpy(+data, &v, sizeof(data));
    return *this;
  }
};

// now traits are redundant, because no first argument (type of pointer) and its always void*
// but for future PtrTraits here
template <unsigned IntBits, typename PtrTraits>
struct pointer_int_pair {
 private:
  noexport::punned_ptr<void*> value;

  static_assert(PtrTraits::num_low_bits_available > 0);
  struct info {
    enum mask_and_shift_constants : uintptr_t {
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

    static intptr_t update_ptr(intptr_t origval, void* Ptr) {
      intptr_t ptr_word = reinterpret_cast<intptr_t>(PtrTraits::to_void_ptr(Ptr));
      assert((ptr_word & ~ptr_bitmask) == 0 && "Pointer is not sufficiently aligned");
      // Preserve all low bits, just update the pointer.
      return ptr_word | (origval & ~ptr_bitmask);
    }

    static intptr_t update_int(intptr_t origval, intptr_t Int) {
      intptr_t int_word = static_cast<intptr_t>(Int);
      assert((int_word & ~int_mask) == 0 && "Integer too large for field");
      // Preserve all bits other than the ones we are updating.
      return (origval & ~shifted_int_mask) | int_word << int_shift;
    }
  };

 public:
  pointer_int_pair() = default;

  pointer_int_pair(void* ptr_val, unsigned int_val) {
    set_ptr_and_int(ptr_val, int_val);
  }

  void* get_ptr() const {
    return info::get_ptr(value);
  }

  unsigned get_int() const {
    return (unsigned)info::get_int(value);
  }

  void set_ptr(void* ptr_val) & {
    value = info::update_ptr(value, ptr_val);
  }

  void set_int(unsigned int_val) & {
    value = info::update_int(value, static_cast<intptr_t>(int_val));
  }

  void set_ptr_and_int(void* ptr_val, unsigned int_val) & {
    value = info::update_int(info::update_ptr(0, ptr_val), intptr_t(int_val));
  }
};

struct pointer_int_pair_fallback {
 private:
  void* ptr;
  unsigned i;

 public:
  pointer_int_pair_fallback() = default;

  pointer_int_pair_fallback(void* ptr_val, unsigned int_val) noexcept {
    set_ptr_and_int(ptr_val, int_val);
  }

  void* get_ptr() const {
    return ptr;
  }

  unsigned get_int() const {
    return i;
  }

  void set_ptr(void* ptr_val) & {
    ptr = ptr_val;
  }

  void set_int(unsigned int_val) & {
    i = int_val;
  }

  void set_ptr_and_int(void* ptr_val, unsigned int_val) & {
    ptr = ptr_val;
    i = int_val;
  }
};

// Determine the number of bits required to store integers with values < n.
// This is ceil(log2(n)).
consteval int bits_required(unsigned n) {
  return n > 1 ? 1 + bits_required((n + 1) / 2) : 0;
}

template <typename CRTP, typename T>
struct box_union_member1 {
  box_union_member1() = default;

  CRTP& self() {
    return static_cast<CRTP&>(*this);
  }
  const CRTP& self() const {
    return static_cast<const CRTP&>(*this);
  }
  box_union_member1(const T& value) {
    self().set_ptr(new T(value));
  }
  box_union_member1(T&& value) {
    self().set_ptr(new T(std::move(value)));
  }
  CRTP& operator=(const T& value) {
    return self() = CRTP(std::in_place_type<T>, value);
  }
  CRTP& operator=(T&& value) {
    return self() = CRTP(std::in_place_type<T>, std::move(value));
  }
  bool operator==(const T& x) const {
    const T* ptr = self().template get_if<T>();
    return ptr && *ptr == x;
  }
  std::strong_ordering operator<=>(const T& x) const {
    const T* ptr = self().template get_if<T>();
    if (!ptr)
      return std::strong_ordering::less;
    return *ptr <=> x;
  }
};

struct void_ptr_traits_t {
  static void* to_void_ptr(void* p) {
    return p;
  }
  static void* from_void_ptr(void* p) {
    return p;
  }
  static constexpr int num_low_bits_available = tgbm::log2_constexpr(__STDCPP_DEFAULT_NEW_ALIGNMENT__);
};

template <typename... Ts>
using box_union_data_t =
    // +1 empty state
    std::conditional_t<(bits_required(sizeof...(Ts) + 1) <= void_ptr_traits_t::num_low_bits_available),
                       pointer_int_pair<bits_required(sizeof...(Ts) + 1), void_ptr_traits_t>,
                       pointer_int_pair_fallback>;

}  // namespace noexport

// passed to visit if box_union is empty (has no value), similar to std::monostate
struct nothing_t {
  std::strong_ordering operator<=>(const nothing_t&) const = default;
};

// behaves similar to std::variant<box<Types>...>,
// but has trivial abi and sizeof(box_union) == sizeof(void*)
// if (sizeof...(Types) - 1) >= log2(__STDCPP_DEFAULT_NEW_ALIGNMENT__),
// then sizeof may be 2 * void*
template <typename... Types>
struct [[clang::trivial_abi]] AA_MSVC_EBO box_union
    : private noexport::box_union_data_t<Types...>,
      noexport::box_union_member1<box_union<Types...>, Types>... {
  // invariant: our pointers allocated by 'new'
  using data_t = noexport::box_union_data_t<Types...>;

 private:
  template <typename CRTP, typename T>
  friend struct noexport::box_union_member1;

  static_assert(is_unique_types<Types...>());
  static_assert((!std::is_same_v<Types, nothing_t> && ...));
  static_assert((std::is_same_v<Types, std::decay_t<Types>> && ...));

  data_t& get_data() {
    return static_cast<data_t&>(*this);
  }
  const data_t& get_data() const {
    return static_cast<const data_t&>(*this);
  }

  template <oneof<Types...> T>
  void set_ptr(T* ptr) noexcept {
    assert(((uintptr_t)ptr % __STDCPP_DEFAULT_NEW_ALIGNMENT__) == 0);
    get_data().set_ptr_and_int(ptr, find_first<T, Types...>());
  }

 public:
  using noexport::box_union_member1<box_union<Types...>, Types>::box_union_member1...;
  using noexport::box_union_member1<box_union<Types...>, Types>::operator=...;
  using noexport::box_union_member1<box_union<Types...>, Types>::operator==...;
  using noexport::box_union_member1<box_union<Types...>, Types>::operator<=>...;

  // assumes, that data is correct (for example) from 'release'
  static box_union from_raw(data_t data) noexcept {
    box_union u;
    u.get_data() = data;
    return u;
  }
  // precondition: 'ptr' may be released with 'delete', correctly aligned
  template <oneof<Types...> T>
  static box_union from_ptr(T* ptr) noexcept {
    box_union u;
    u.set_ptr(ptr);
    return u;
  }
  // returns such data, which when used in 'from_raw' creates empty box_union
  static data_t empty_state() noexcept {
    return data_t(nullptr, sizeof...(Types));
  }

  // postcondition: index() == sizeof...(Types), stored 'nothing_t'
  box_union() noexcept : data_t(empty_state()) {
  }
  box_union(std::nullptr_t) noexcept : box_union() {
  }
  ~box_union() {
    reset();
  }

  box_union(const box_union& other) : box_union() {
    other.visit_ptr(matcher{[&](nothing_t) {},
                            [&]<typename T>(const T* x) {
                              if (x)
                                emplace<T>(*x);
                            }});
  }
  box_union(box_union&& other) noexcept : data_t(other.get_data()) {
    other.get_data() = empty_state();
  }

  template <typename T, typename... Args>
  box_union(std::in_place_type_t<T>, Args&&... args)
      : data_t(new T(std::forward<Args>(args)...), find_first<T, Types...>()) {
    static_assert(contains_type<T, Types...>());
  }
  template <size_t I, typename... Args>
  box_union(std::in_place_index_t<I>, Args&&... args)
      : box_union<element_at_t<I, Types...>>(std::forward<Args>(args)...) {
  }
  template <typename T, typename... Args>
  T& emplace(Args&&... args) {
    static_assert(contains_type<T, Types...>());
    *this = box_union(std::in_place_type<T>, std::forward<Args>(args)...);
    return *get_if<T>();
  }
  template <size_t I, typename... Args>
  auto& emplace(Args&&... args) {
    return emplace<element_at_t<I, Types...>>(std::forward<Args>(args)...);
  }

  [[nodiscard]] data_t release() noexcept {
    return std::exchange(get_data(), empty_state());
  }
  // postcondition: is_null()
  void reset() noexcept {
    static_assert((sizeof(Types) && ...));
    visit_ptr(matcher{[](nothing_t) {},
                      [](auto* p) {
                        if (p)
                          delete p;
                      }});
    get_data() = empty_state();
    assert(is_null());
  }

  void swap(box_union& other) noexcept {
    std::swap(get_data(), other.get_data());
  }
  friend void swap(box_union& a, box_union& b) noexcept {
    a.swap(b);
  }
  box_union& operator=(const box_union& other) {
    *this = box_union(other);
    return *this;
  }
  box_union& operator=(box_union&& other) noexcept {
    swap(other);
    return *this;
  }
  box_union& operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }

  // from boxes

  template <oneof<Types...> T>
  box_union(box<T>&& b) noexcept {
    // detect cases when Types... contain box<int>, int, ambigious
    static_assert(!contains_type<box<T>, Types...>());
    set_ptr(b.release());
  }
  template <oneof<Types...> T>
  box_union(const box<T>& b) : box_union(box<T>(b)) {
  }
  template <oneof<Types...> T>
  box_union& operator=(box<T>&& b) noexcept {
    // detect cases when Types... contain box<int>, int, ambigious
    static_assert(!contains_type<box<T>, Types...>());
    reset();
    set_ptr(b.release());
    return *this;
  }
  template <oneof<Types...> T>
  box_union& operator=(const box<T>& b) noexcept {
    return *this = box<T>(b);
  }

  // no matter what stored, checks if it nullptr
  [[nodiscard]] bool is_null() const noexcept {
    return get_data().get_ptr() == nullptr;
  }
  explicit operator bool() const noexcept {
    return !is_null();
  }

  // postcondition: 0 <= index() <= sizeof...(Types)
  // index() == sizeof...(Types) means 'nothing_t' stored
  [[nodiscard]] size_t index() const noexcept {
    assert(get_data().get_int() <= sizeof...(Types));
    return size_t(get_data().get_int());
  }

  // Note: it is possible, that stored T but returned nullptr, because stored (T*)nullptr
  template <typename T>
  [[nodiscard]] T* get_if() noexcept {
    static_assert(contains_type<T, Types...>());
    if (index() != find_first<T, Types...>())
      return nullptr;
    return static_cast<T*>(get_data().get_ptr());
  }
  // Note: it is possible, that stored T but returned nullptr, because stored (T*)nullptr
  template <typename T>
  [[nodiscard]] const T* get_if() const noexcept {
    static_assert(contains_type<T, Types...>());
    if (index() != find_first<T, Types...>())
      return nullptr;
    return static_cast<const T*>(get_data().get_ptr());
  }
  template <size_t I>
  auto* get_if() noexcept {
    return get_if<element_at_t<I, Types...>>();
  }
  template <size_t I>
  const auto* get_if() const noexcept {
    return get_if<element_at_t<I, Types...>>();
  }

  // passes 'nothing_t' empty
  template <typename V>
  decltype(auto) visit_ptr(V&& vtor) {
    auto i = index();
    if (is_null())
      return vtor(nothing_t{});
    assert(i < sizeof...(Types) && "internal logic failure");
    return visit_index<sizeof...(Types) - 1>([&]<size_t I>() -> decltype(auto) { return vtor(get_if<I>()); },
                                             i);
  }
  // passes 'nothing_t' if empty
  template <typename V>
  decltype(auto) visit_ptr(V&& vtor) const {
    auto i = index();
    if (is_null())
      return vtor(nothing_t{});
    assert(i < sizeof...(Types) && "internal logic failure");
    return visit_index<sizeof...(Types) - 1>([&]<size_t I>() -> decltype(auto) { return vtor(get_if<I>()); },
                                             i);
  }

  // no matter what stored, checks if it nullptr
  bool operator==(std::nullptr_t) const noexcept {
    return is_null();
  }
  friend bool operator==(const box_union& l, const box_union& r) noexcept {
    if (l.index() != r.index())
      return false;
    return l.visit_ptr(matcher{[](nothing_t) { return true; },
                               [&]<typename T>(const T* p) {
                                 const T* lptr = p;
                                 const T* rptr = r.template get_if<T>();
                                 if (!lptr || !rptr)
                                   return !lptr && !rptr;
                                 return *lptr == *rptr;
                               }});
  }
  friend std::strong_ordering operator<=>(const box_union& l, const box_union& r) noexcept {
    if (!l && !r)
      return std::strong_ordering::equal;
    if (!l)
      return std::strong_ordering::less;
    if (!r)
      return std::strong_ordering::greater;
    if (l.index() != r.index())
      return l.index() <=> r.index();
    return l.visit_ptr(matcher{[](nothing_t) -> std::strong_ordering { KELCORO_UNREACHABLE; },
                               [&]<typename T>(const T* p) { return *p <=> *r.template get_if<T>(); }});
  }
};

template <typename T, typename... Ts>
T* get_if(tgbm::box_union<Ts...>* u) {
  if (!u)
    return nullptr;
  return u->template get_if<T>();
}
template <typename T, typename... Ts>
const T* get_if(const tgbm::box_union<Ts...>* u) {
  if (!u)
    return nullptr;
  return u->template get_if<T>();
}

template <size_t I, typename... Ts>
const auto* get_if(tgbm::box_union<Ts...>* u) noexcept {
  if (!u)
    return nullptr;
  return u->template get_if<I>();
}
template <size_t I, typename... Ts>
[[nodiscard]] const auto* get_if(const tgbm::box_union<Ts...>* u) noexcept {
  if (!u)
    return nullptr;
  return u->template get_if<I>();
}

}  // namespace tgbm

#include <anyany/noexport/file_end.hpp>
