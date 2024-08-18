#pragma once
#include <boost/pfr.hpp>
#include <tgbm/tools/constexpr_string.h>

#define FWD(X) std::forward<decltype(X)>(X)

namespace pfr_extension {

template <std::size_t Index, tgbm::ce::string Name>
struct field_info {
  static constexpr auto index = Index;
  static constexpr auto name = Name;
};

namespace details {
template <std::size_t I>
constexpr void one_field_consume(auto& t, auto& functor) {
  using T = std::remove_cvref_t<decltype(t)>;
  using Info = field_info<I, boost::pfr::get_name<I, T>()>;
  functor.template operator()<Info>(boost::pfr::get<I>(t));
}
template <std::size_t... I>
constexpr void visit_object_helper(auto&& t, auto&& functor, std::integer_sequence<std::size_t, I...>) {
  (one_field_consume<I>(t, functor), ...);
}
}  // namespace details

//<typename Info, typename F>
constexpr void visit_object(auto&& t, auto&& functor) {
  using T = std::remove_cvref_t<decltype(t)>;
  constexpr std::size_t N = boost::pfr::tuple_size_v<T>;
  auto seq = std::make_index_sequence<N>{};
  details::visit_object_helper(FWD(t), FWD(functor), seq);
}
}  // namespace pfr_extension

#undef FWD
