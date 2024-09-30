#pragma once
#include <optional>
#include <string_view>
#include <type_traits>
#include <tgbm/api/optional.hpp>

namespace tgbm {
template <typename T>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<std::optional<T>> : std::true_type {};

template <typename T>
struct is_optional<api::optional<T>> : std::true_type {};

template <typename T>
constexpr auto is_optional_v = is_optional<T>::value;

template <typename T, typename... Types>
concept any_of = (std::is_same_v<T, Types> || ...);

template <typename T>
concept char_type_like = any_of<T, char, signed char, unsigned char, char8_t, char16_t, char32_t, wchar_t>;

template <typename T>
concept numeric = std::is_integral_v<T> && !char_type_like<T> && !std::is_same_v<T, bool>;

template <typename T>
concept string_like = std::convertible_to<T, std::string_view>;

template <typename T>
concept array_like = std::ranges::range<T> && !string_like<T>;

template <typename T>
concept aggregate = std::is_aggregate_v<T>;

template <typename T>
concept nesting = aggregate<T> || array_like<T>;

namespace details {
template <typename T>
concept common_api_type = aggregate<T> && requires(std::string_view sv) {
  { T::is_optional_field(sv) } -> std::same_as<bool>;
};

template <typename T>
concept discriminated_api_type = aggregate<T> && requires(std::string_view sv) {
  {
    T::discriminate(sv, []<typename>() {})
  };
};

template <typename T>
concept oneof_field_api_type = aggregate<T> && requires(std::string_view sv) {
  {
    T::discriminate_field(sv, []<typename>() {})
  };
};
}  // namespace details

template <typename T>
concept common_api_type = aggregate<T> && details::common_api_type<std::remove_cvref_t<T>>;

template <typename T>
concept discriminated_api_type = aggregate<T> && details::discriminated_api_type<std::remove_cvref_t<T>>;

template <typename T>
concept oneof_field_api_type = aggregate<T> && details::oneof_field_api_type<std::remove_cvref_t<T>>;

template <typename T>
concept api_type = common_api_type<T> || discriminated_api_type<T> || oneof_field_api_type<T>;

}  // namespace tgbm
