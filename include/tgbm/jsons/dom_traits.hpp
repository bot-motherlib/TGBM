#pragma once

#include <concepts>
#include <cstdint>
#include <string_view>
#include <ranges>

namespace tgbm::json {

template <typename Traits, typename Json = typename Traits::type>
concept dom_traits = requires(const Json& json) {
  requires std::same_as<typename Traits::type, Json>;

  { Traits::is_bool(json) } -> std::same_as<bool>;

  { Traits::is_integer(json) } -> std::same_as<bool>;

  { Traits::is_uinteger(json) } -> std::same_as<bool>;

  { Traits::is_floating(json) } -> std::same_as<bool>;

  { Traits::is_string(json) } -> std::same_as<bool>;

  { Traits::is_object(json) } -> std::same_as<bool>;

  { Traits::is_array(json) } -> std::same_as<bool>;

  { Traits::is_object(json) } -> std::same_as<bool>;

  { Traits::is_null(json) } -> std::same_as<bool>;

  { Traits::get_bool(json) } -> std::same_as<bool>;

  { Traits::on_error() };

  { Traits::get_integer(json) } -> std::same_as<std::int64_t>;

  { Traits::get_uinteger(json) } -> std::same_as<std::uint64_t>;

  { Traits::get_string(json) } -> std::convertible_to<std::string_view>;

  { Traits::get_floating(json) } -> std::same_as<double>;

  { Traits::find_field(json, std::string_view{}) };

  { Traits::element_by_index(json, size_t{}) } -> std::same_as<const Json&>;

  { Traits::size(json) } -> std::same_as<std::size_t>;

  // for (auto&& [key, value] : Traits::member_range(json)) {}
  { Traits::member_range(json) } -> std::ranges::range;
};

template <typename Json>
struct dom_traits_for {};

template <typename Json>
concept dom = dom_traits<dom_traits_for<std::remove_cvref_t<Json>>>;

}  // namespace tgbm::json
