#pragma once

#include <string_view>
#include <type_traits>

namespace tgbm {

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

namespace details {

template <typename T>
concept common_api_type = aggregate<T> && requires {
  { T::is_mandatory_field(std::string_view{}) } -> std::same_as<bool>;
};

template <typename T>
concept discriminated_api_type = aggregate<T> && requires(const T& t) {
  {
    T::discriminate(std::string_view{}, []<typename>() {})
  };
  { t.discriminator_now() } -> std::same_as<std::string_view>;
  { t.data };
};

template <typename T>
concept oneof_field_api_type = aggregate<T> && requires(const T& t) {
  {
    T::discriminate_field(std::string_view{}, []<typename>() {})
  };
  { t.discriminator_now() } -> std::same_as<std::string_view>;
  { t.data };
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
