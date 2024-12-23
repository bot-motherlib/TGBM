#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <random>
#include <type_traits>
#include <unordered_map>
#include <fstream>

#include <tgbm/utils/box.hpp>
#include <tgbm/utils/pfr_extension.hpp>
#include <tgbm/utils/traits.hpp>
#include <tgbm/logger.hpp>
#include <tgbm/jsons/boostjson_serialize_dom.hpp>

namespace fuzzing {

constexpr size_t max = size_t(-1);

struct Config {
  std::string path;
  bool pretty = true;

  double eps = 0.01;

  double expand_chance = 0.5;
  double nesting_chance = 0.5;

  size_t min_fields_total = 0;
  size_t max_fields_total = max;

  size_t min_fields_per_object = 0;
  size_t max_fields_per_object = max;

  size_t max_nesting = 16;
  size_t max_size_array = 16;

  int64_t min_int = 0;
  int64_t max_int = 10'000;

  double min_double = -200000;
  double max_double = 200000;
};

struct LocalInfo {
  size_t cur_nesting = 0;
  size_t cur_fields_in_object = 0;
};

struct GlobalInfo {
  size_t cur_fields_total = 0;
};

struct Context {
  Config config;
  LocalInfo local;
  GlobalInfo& global;

  Context from_local(LocalInfo local) {
    return Context{.config = config, .local = local, .global = global};
  }
};

template <typename T>
struct randomizer {};

inline void pretty_print(std::ostream& os, const boost::json::value& jv, std::string* indent = nullptr) {
  namespace json = boost::json;
  std::string indent_;
  if (!indent)
    indent = &indent_;
  switch (jv.kind()) {
    case json::kind::object: {
      os << "{\n";
      indent->append(4, ' ');
      auto const& obj = jv.get_object();
      if (!obj.empty()) {
        auto it = obj.begin();
        for (;;) {
          os << *indent << json::serialize(it->key()) << " : ";
          pretty_print(os, it->value(), indent);
          if (++it == obj.end())
            break;
          os << ",\n";
        }
      }
      os << "\n";
      indent->resize(indent->size() - 4);
      os << *indent << "}";
      break;
    }

    case json::kind::array: {
      os << "[\n";
      indent->append(4, ' ');
      auto const& arr = jv.get_array();
      if (!arr.empty()) {
        auto it = arr.begin();
        for (;;) {
          os << *indent;
          pretty_print(os, *it, indent);
          if (++it == arr.end())
            break;
          os << ",\n";
        }
      }
      os << "\n";
      indent->resize(indent->size() - 4);
      os << *indent << "]";
      break;
    }

    case json::kind::string: {
      os << json::serialize(jv.get_string());
      break;
    }

    case json::kind::uint64:
    case json::kind::int64:
    case json::kind::double_:
      os << jv;
      break;

    case json::kind::bool_:
      if (jv.get_bool())
        os << "true";
      else
        os << "false";
      break;

    case json::kind::null:
      os << "null";
      break;
  }

  if (indent->empty())
    os << "\n";
}

template <typename T>
inline std::string make_json(const Config& config, auto&& generator) {
  GlobalInfo info;
  Context context{.config = config, .global = info};
  auto t = randomizer<T>::generate(context, generator);
  boost::json::value j = boost::json::value_from(t);
  std::string result;
  if (config.pretty) {
    std::stringstream ss;
    pretty_print(ss, j);
    result = std::move(ss).str();
  } else {
    result = boost::json::serialize(j);
  }
  return result;
}

template <typename T, size_t N>
inline std::string make_json_array(const Config& config, auto&& generator) {
  GlobalInfo info;
  Context context{.config = config, .global = info};
  std::vector<T> t;
  t.reserve(N);
  while (t.size() != N) {
    auto obj = randomizer<T>::generate(context, generator);
    t.emplace_back(std::move(obj));
    context.global.cur_fields_total += 1;
  }
  boost::json::value j = boost::json::value_from(t);
  std::string result;
  if (config.pretty) {
    std::stringstream ss;
    pretty_print(ss, j);
    result = std::move(ss).str();
  } else {
    result = boost::json::serialize(j);
  }
  return result;
}

struct TransparentHash : std::hash<std::string_view> {
  using is_transparent = int;
};

struct Storage {
  Storage(bool use_random) {
    if (use_random) {
      generator = std::mt19937_64(std::random_device{}());
    } else {
      generator = std::mt19937_64(42);
    }
  }

  template <typename T>
  void Add(std::string name, Config config) {
    std::filesystem::path path(config.path);
    TGBM_LOG_INFO("Add: {}", std::filesystem::absolute(path).generic_string());
    std::optional<std::string> json;
    if (std::filesystem::exists(path)) {
      std::fstream file(path);
      if (file.is_open()) {
        std::getline(file, json.emplace(), '\0');
      }
    } else if (!std::filesystem::exists(path.parent_path())) {
      std::filesystem::create_directories(path.parent_path());
    }

    if (!json) {
      GlobalInfo info;
      json = make_json<T>(config, generator);
      std::ofstream file;
      file.exceptions(std::fstream::failbit | std::fstream::badbit);
      file.open(path);
      if (!file.is_open()) {
        throw std::runtime_error("FUCK THIS SHIT");
      }
      file << *json;
      file.flush();
      file.close();
    }

    jsons_[std::move(name)] = std::move(json.value());
  }

  template <typename T, size_t N>
  void AddArray(std::string name, Config config) {
    std::filesystem::path path(config.path);
    TGBM_LOG_INFO("AddArray: {}", std::filesystem::absolute(path).generic_string());
    std::optional<std::string> json;
    if (std::filesystem::exists(path)) {
      std::fstream file(path);
      if (file.is_open()) {
        std::getline(file, json.emplace(), '\0');
      }
    } else if (!std::filesystem::exists(path.parent_path())) {
      std::filesystem::create_directories(path.parent_path());
    }

    if (!json) {
      GlobalInfo info;
      json = make_json_array<T, N>(config, generator);
      std::ofstream file;
      file.exceptions(std::fstream::failbit | std::fstream::badbit);
      file.open(path);
      if (!file.is_open()) {
        throw std::runtime_error("FUCK THIS SHIT");
      }
      file << *json;
      file.flush();
      file.close();
    }

    jsons_[std::move(name)] = std::move(json.value());
  }

  std::string_view operator[](std::string_view path) const {
    auto it = jsons_.find(path);
    if (it == jsons_.end()) {
      throw std::runtime_error(fmt::format("Not found json: {}", path));
    }
    return it->second;
  }

 private:
  std::unordered_map<std::string, std::string, TransparentHash, std::ranges::equal_to> jsons_;
  std::mt19937_64 generator{std::random_device{}()};
};

inline Storage& GetMutableStorage(bool use_random = false) {
  static Storage storage(use_random);
  return storage;
}

inline const Storage& GetStorage(bool use_random = false) {
  return GetMutableStorage(use_random);
}

template <std::integral T>
struct randomizer<T> {
  static constexpr bool is_nesting = false;
  static T generate(Context context, auto&& generator) {
    T min = 0;
    T max = 10'000;

    std::uniform_real_distribution<> dist(0, 1);
    double n = dist(generator);
    T number = min + (max - min) * n * n * n;

    context.global.cur_fields_total += 1;

    return number;
  }
};

template <>
struct randomizer<tgbm::api::Integer> {
  static constexpr bool is_nesting = false;
  static tgbm::api::Integer generate(Context context, auto&& generator) {
    context.config.max_int = tgbm::api::Integer::max;
    return tgbm::api::Integer(randomizer<decltype(tgbm::api::Integer::value)>::generate(context, generator));
  }
};

template <>
struct randomizer<bool> {
  static constexpr bool is_nesting = false;
  static bool generate(auto&& generator) {
    std::uniform_int_distribution<int> distribution(0, 1);
    auto val = distribution(generator);
    return val == 1;
  }
  static bool generate(Context context, auto&& generator) {
    std::uniform_int_distribution<int> distribution(0, 1);
    auto val = distribution(generator);
    context.global.cur_fields_total += 1;
    return val == 1;
  }
};

template <>
struct randomizer<tgbm::api::Double> {
  static constexpr bool is_nesting = false;
  static bool generate(Context context, auto&& generator) {
    double min = context.config.min_double;
    double max = context.config.max_double;
    // Вычисляем среднее и стандартное отклонение для нормального распределения
    double mean = (static_cast<double>(min) + static_cast<double>(max)) / 2.0;
    double stddev =
        (static_cast<double>(max) - static_cast<double>(min)) / 6.0;  // Для охвата ~99.7% значений

    std::normal_distribution<double> distr(mean, stddev);
    context.global.cur_fields_total += 1;
    return distr(generator);
  }
};

template <>
struct randomizer<std::string> {
  static constexpr bool is_nesting = false;
  static std::string generate(Context context, auto&& generator) {
    std::uniform_int_distribution<size_t> distribution(size_t('A'), size_t('Z'));
    std::uniform_int_distribution<size_t> size_distr(0, 15);
    std::uint8_t size = size_distr(generator);
    std::string result;
    result.resize(size, '\0');

    for (char& ch : result) {
      ch = distribution(generator);
    }
    context.global.cur_fields_total += 1;
    return result;
  }
};

template <>
struct randomizer<tgbm::const_string> {
  static constexpr bool is_nesting = false;
  static tgbm::const_string generate(Context context, auto&& generator) {
    return randomizer<std::string>::generate(context, generator);
  }
};

template <typename T>
struct randomizer<std::optional<T>> {
  static constexpr bool is_nesting = randomizer<T>::is_nesting;
  static std::optional<T> generate(Context context, auto&& generator) {
    return std::optional<T>(randomizer<T>::generate(context, generator));
  }
};
#if 0
template <typename T>
struct randomizer<tgbm::api::optional<T>> {
  static constexpr bool is_nesting = randomizer<T>::is_nesting;
  static tgbm::api::optional<T> generate(Context context, auto&& generator) {
    return tgbm::api::optional<T>(randomizer<T>::generate(context, generator));
  }
};
#endif
template <>
struct randomizer<tgbm::api::True> {
  static constexpr bool is_nesting = false;
  static tgbm::api::True generate(Context context, auto&&) {
    context.global.cur_fields_total += 1;
    return {};
  }
};

template <>
struct randomizer<tgbm::nothing_t> {
  static constexpr bool is_nesting = false;
  static tgbm::nothing_t generate(Context context, auto&&) {
    context.global.cur_fields_total += 1;
    return {};
  }
};

template <typename T>
struct randomizer<tgbm::box<T>> {
  static constexpr bool is_nesting = randomizer<T>::is_nesting;
  static tgbm::box<T> generate(Context context, auto&& generator) {
    return tgbm::box<T>{randomizer<T>::generate(context, generator)};
  }
};

template <typename T>
struct randomizer<std::vector<T>> {
  static constexpr bool is_nesting = true;

  static std::vector<T> generate(Context context, auto&& generator) {
    LocalInfo local{.cur_nesting = context.local.cur_nesting + 1};

    std::vector<T> result;

    if (randomizer<T>::is_nesting && local.cur_nesting >= context.config.max_nesting) {
      return result;
    }

    std::bernoulli_distribution exp_distr(context.config.expand_chance);

    bool need_expand = result.size() < context.config.max_size_array ? exp_distr(generator) : false;
    while (need_expand) {
      result.emplace_back(randomizer<T>::generate(context.from_local(local), generator));
      context.global.cur_fields_total += 1;
      need_expand = result.size() < context.config.max_size_array ? exp_distr(generator) : false;
    }
    return result;
  }
};

template <tgbm::common_api_type T>
struct randomizer<T> {
  static constexpr bool is_nesting = true;

  static T generate(Context context, auto&& generator) {
    T t;
    LocalInfo local{.cur_nesting = context.local.cur_nesting + 1};

    bool generate_nesting = [&]() {
      auto distr = std::bernoulli_distribution(context.config.nesting_chance);
      return distr(generator);
    }();

    pfr_extension::visit_object(t, [&]<typename Info, typename Field>(Field& field) {
      constexpr std::string_view name = Info::name.AsStringView();
      constexpr bool is_optional = !T::is_mandatory_field(name);
      auto need_generate = [&]() -> bool {
        if (!is_optional) {
          return true;
        }
        if (local.cur_nesting >= context.config.max_nesting) {
          return false;
        }
        if (context.config.min_fields_total > context.global.cur_fields_total) {
          return true;
        }
        if (context.config.min_fields_per_object > local.cur_fields_in_object) {
          return true;
        }
        if (context.config.max_fields_per_object < local.cur_fields_in_object) {
          return false;
        }
        if (context.config.max_fields_total < context.global.cur_fields_total) {
          return false;
        }
        if (randomizer<Field>::is_nesting && !generate_nesting) {
          return false;
        } else {
          auto distr = std::bernoulli_distribution(context.config.expand_chance);
          return distr(generator);
        }
      }();
      if (!need_generate) {
        return;
      }
      field = randomizer<Field>::generate(
          Context{
              .config = context.config,
              .local = local,
              .global = context.global,
          },
          generator);
      local.cur_fields_in_object += 1;
      context.global.cur_fields_total += 1;
    });
    return t;
  }
};

template <tgbm::discriminated_api_type T>
struct randomizer<T> {
  static constexpr bool is_nesting = true;

  static T generate(Context context, auto&& generator) {
    using Data = decltype(T::data);
    T t;
    LocalInfo local{.cur_nesting = context.local.cur_nesting + 1, .cur_fields_in_object = 1};
    context.global.cur_fields_total += 1;
    std::uniform_int_distribution<int> sub_one_of_dist(0, T::variant_size - 1);
    tgbm::visit_index<T::variant_size - 1>(
        [&]<size_t I>() {
          using SubOneOf = tgbm::box_union_element_t<Data, I>;
          t.data = randomizer<SubOneOf>::generate(
              Context{
                  .config = context.config,
                  .local = local,
                  .global = context.global,
              },
              generator);
        },
        sub_one_of_dist(generator));

    assert(t.data);
    return t;
  }
};

template <tgbm::oneof_field_api_type T>
struct randomizer<T> {
  static constexpr bool is_nesting = true;

  static T generate(Context context, auto&& generator) {
    T t;
    LocalInfo local{.cur_nesting = context.local.cur_nesting + 1, .cur_fields_in_object = 1};
    context.global.cur_fields_total += 1;
    pfr_extension::visit_object<tgbm::oneof_field_utils::N<T>>(
        t, [&]<typename Info, typename Field>(Field& field) {
          constexpr std::string_view name = Info::name.AsStringView();
          field = randomizer<Field>::generate(
              Context{
                  .config = context.config,
                  .local = local,
                  .global = context.global,
              },
              generator);
          local.cur_fields_in_object += 1;
          context.global.cur_fields_total += 1;
        });
    std::uniform_int_distribution<int> sub_one_of_dist(0, T::variant_size - 1);
    using Data = decltype(T::data);
    tgbm::visit_index<T::variant_size - 1>(
        [&]<size_t I>() {
          using SubOneOf = tgbm::box_union_element_t<Data, I>;
          using raw = decltype(SubOneOf::value);
          t.data = SubOneOf{randomizer<raw>::generate(
              Context{
                  .config = context.config,
                  .local = local,
                  .global = context.global,
              },
              generator)};
        },
        sub_one_of_dist(generator));
    assert(t.data);
    return t;
  }
};

}  // namespace fuzzing
