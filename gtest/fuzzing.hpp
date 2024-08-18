#pragma once
#include <optional>
#include <ranges>
#include <string>
#include <random>
#include <type_traits>
#include <numeric>
#include "tgbm/tools/box.hpp"
#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/tools/traits.hpp>

namespace fuzzing {
inline auto make_generator(std::size_t hash) {
  return std::mt19937_64(hash);
}

template <typename T>
struct randomizer {};

template <typename T>
inline auto make_random_object(auto&& generator) {
  return randomizer<T>::generate(generator);
}

template <std::integral T>
struct randomizer<T> {
  static T generate(auto&& generator) {
    std::uniform_int_distribution<T> distribution(std::numeric_limits<T>::min(),
                                                  std::numeric_limits<T>::max());
    return distribution(generator);
  }
};

template <>
struct randomizer<bool> {
  static bool generate(auto&& generator) {
    std::uniform_int_distribution<int> distribution(0, 1);
    auto val = distribution(generator);
    return val == 1;
  }
};

template <>
struct randomizer<char> {
  static char generate(auto&& generator) {
    std::uniform_int_distribution<char> distribution(' ', '~');
    return distribution(generator);
  }
};

template <>
struct randomizer<std::string> {
  static std::string generate(auto&& generator) {
    auto size = randomizer<std::uint8_t>::generate(generator);
    std::string result;
    result.resize(size, '\0');
    for (char& ch : result) {
      ch = randomizer<char>::generate(generator);
    }
    return result;
  }
};

template <typename T>
struct randomizer<std::optional<T>> {
  static std::optional<T> generate(auto&& generator) {
    bool has_value = randomizer<bool>::generate(generator);
    if (!has_value) {
      return std::nullopt;
    }
    return std::optional<T>(randomizer<T>::generate(generator));
  }
};

template <tgbm::aggregate T>
struct randomizer<T> {
  static T generate(auto&& generator) {
    T t;
    pfr_extension::visit_object(t, [&]<typename Info, typename Field>(Field& field) {
      field = randomizer<Field>::generate(generator);
    });
    return t;
  }
};

template <typename T>
struct randomizer<tgbm::box<T>> {
  static tgbm::box<T> generate(auto&& generator) {
    bool has_value = randomizer<bool>::generate(generator);
    if (!has_value) {
      return nullptr;
    }
    return randomizer<T>::generate(generator);
  }
};

template <>
struct randomizer<std::true_type> {
  static std::true_type generate(auto&&) {
    return {};
  }
};

template <typename T>
constexpr auto range(std::size_t begin, std::size_t end, auto&& generator) {
  return std::ranges::views::iota(begin, end) | std::views::transform([&](std::size_t hash) {
           return std::make_pair(hash, make_random_object<T>(generator));
         });
}
}  // namespace tgbm::random
