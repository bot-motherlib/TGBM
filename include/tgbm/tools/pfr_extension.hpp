#pragma once
#include <boost/pfr.hpp>
#include <tgbm/tools/constexpr_string.h>
#include <tgbm/tools/StringTools.h>
#include <tgbm/tools/meta.hpp>

#define FWD(X) std::forward<decltype(X)>(X)

namespace pfr_extension {

template <std::size_t Index, tgbm::ce::string Name>
struct field_info {
  static constexpr auto index = Index;
  static constexpr auto name = Name;
};

namespace details {

template <std::size_t... I>
constexpr void visit_object_helper(auto&& t, auto&& functor, std::integer_sequence<std::size_t, I...>) {
  using T = std::remove_cvref_t<decltype(t)>;
  auto one_field = [&]<std::size_t J>(std::index_sequence<J>) {
    using Info = field_info<J, boost::pfr::get_name<J, T>()>;
    functor.template operator()<Info>(boost::pfr::get<J>(FWD(t)));
  };
  (one_field(std::index_sequence<I>{}), ...);
}

template <typename T, std::size_t... I>
constexpr void visit_struct_helper(auto&& functor, std::integer_sequence<std::size_t, I...>) {
  auto one_field = [&]<std::size_t J>(std::index_sequence<J>) {
    using Info = field_info<J, boost::pfr::get_name<J, T>()>;
    using Field = boost::pfr::tuple_element_t<J, T>;
    functor.template operator()<Info, Field>();
  };
  (one_field(std::index_sequence<I>{}), ...);
}

template <tgbm::ce::string Name, typename T>
std::size_t element_index() {
  constexpr auto names = boost::pfr::names_as_array<T>();
  std::size_t index = 0;
  for (std::string_view name : names) {
    if (name == Name.AsStringView()) {
      return index;
    }
    index++;
  }
  return -1;
}

}  // namespace details

// typename Info, typename F
template <std::size_t N = std::size_t(-1)>
constexpr void visit_object(auto&& t, auto&& functor) {
  using T = std::remove_cvref_t<decltype(t)>;
  constexpr auto default_size = boost::pfr::tuple_size_v<T>;
  auto seq = std::make_index_sequence < N != -1 ? N : default_size > {};
  details::visit_object_helper(FWD(t), FWD(functor), seq);
}

template <typename T, std::size_t N = boost::pfr::tuple_size_v<std::remove_cvref_t<T>>>
constexpr void visit_struct(auto&& functor) {
  using U = std::remove_cvref_t<T>;

  auto seq = std::make_index_sequence<N>{};
  details::visit_struct_helper<U>(FWD(functor), seq);
}

template <tgbm::ce::string Name, typename T>
constexpr std::size_t tuple_element_index_v = details::element_index<Name, T>();

template <tgbm::ce::string Name, typename T>
constexpr std::size_t has_element_v = details::element_index<Name, T>() <= boost::pfr::tuple_size_v<T>;

template <std::size_t I, typename T>
consteval std::string_view get_name() {
  return boost::pfr::get_name<I, T>();
}

template <std::size_t I, typename T>
constexpr std::string_view element_name_v = get_name<I, T>();

template <typename T>
consteval bool has_element(std::string_view name) {
  constexpr auto names = boost::pfr::names_as_array<T>();
  for (std::string_view n : names) {
    if (name == n) {
      return true;
    }
  }
  return false;
}

template <typename T, typename R, std::size_t Size = boost::pfr::tuple_size_v<T>>
constexpr R visit_struct_field(std::string_view field_name, auto&& known, auto&& unknown) {
  constexpr std::size_t kUnknown = std::size_t(-1);
  std::size_t index = [&]<std::size_t... I>(std::index_sequence<I...>) {
    tgbm::utils::string_switch<std::size_t> s(field_name);
    return (s | ... | tgbm::utils::string_switch<std::size_t>::case_t(element_name_v<I, T>, I))
        .or_default(kUnknown);
  }(std::make_index_sequence<Size>{});
  if (index != kUnknown) [[likely]] {
    return tgbm::visit_index<Size - 1>(known, index);
  }
  return unknown();
}

}  // namespace pfr_extension

#undef FWD
