#include "benchmark/benchmark.h"
#include "fuzzing.hpp"
#include "types.hpp"
#include "tgbm/api/types/all.hpp"
#include "tgbm/tools/json_tools/all.hpp"
#include "tgbm/tools/json_tools/parse_dom/all.hpp"

template <typename T>
void execute_handler_rapid(std::string_view json) {
  benchmark::DoNotOptimize(tgbm::json::rapid::parse_handler<T>(json));
}

template <typename T>
void execute_dom_rapid(std::string_view json) {
  benchmark::DoNotOptimize(tgbm::json::rapid::parse_dom<T>(json));
}

template <typename T>
void execute_ignore_handler_rapid(std::string_view json) {
  benchmark::DoNotOptimize(tgbm::json::rapid::parse_ignore_handler(json));
}

template <typename T>
void execute_handler_boost(std::string_view json) {
  benchmark::DoNotOptimize(tgbm::json::boost::parse_handler<T>(json));
}

template <typename T>
void execute_dom_boost(std::string_view json) {
  benchmark::DoNotOptimize(tgbm::json::boost::parse_dom<T>(json));
}

template <typename T>
void execute_ignore_handler_boost(std::string_view json) {
  benchmark::DoNotOptimize(tgbm::json::boost::parse_ignore_handler(json));
}

template <typename T>
void execute_generator_boost(std::string_view json) {
  benchmark::DoNotOptimize(tgbm::json::boost::parse_generator<T>(json));
}

template <typename T>
void execute_stream_parser(std::string_view json) {
  T result;
  tgbm::json::stream_parser parser(result);
  do {
    parser.feed(json.substr(0, 1024), json.size() <= 1024);
    json.remove_prefix(std::min<int>(1024, json.size()));
  } while (!json.empty());
  benchmark::DoNotOptimize(result);
}

#define BENCHMARK_FILE_ALG_IMPL(TYPE, NAME, ALG) \
  void NAME##_##ALG(benchmark::State& state) {   \
    auto json = fuzzing::GetStorage()[#NAME];    \
    for (auto _ : state) {                       \
      execute_##ALG<TYPE>(json);                 \
    }                                            \
  }                                              \
  BENCHMARK(NAME##_##ALG)->Name(#NAME " [" #ALG "]");

#define BENCHMARK_FILE_ALG(TYPE, NAME, ALG) BENCHMARK_FILE_ALG_IMPL(TYPE, NAME, ALG)

// clang-format off
#define BENCHMARK_FILE(TYPE, NAME)                      \
  BENCHMARK_FILE_ALG(TYPE, NAME, ignore_handler_rapid); \
  BENCHMARK_FILE_ALG(TYPE, NAME, ignore_handler_boost)  \
  BENCHMARK_FILE_ALG(TYPE, NAME, dom_rapid);            \
  BENCHMARK_FILE_ALG(TYPE, NAME, dom_boost);            \
  BENCHMARK_FILE_ALG(TYPE, NAME, handler_rapid);        \
  BENCHMARK_FILE_ALG(TYPE, NAME, handler_boost);        \
  BENCHMARK_FILE_ALG(TYPE, NAME, generator_boost)       \
  BENCHMARK_FILE_ALG(TYPE, NAME, stream_parser)
// clang-format on

BENCHMARK_FILE(tgbm::api::User, SimpleUser);
BENCHMARK_FILE(tgbm::api::Message, SmallMessage);
BENCHMARK_FILE(tgbm::api::Message, MediumMessage);
BENCHMARK_FILE(tgbm::api::Message, HardMessage);
BENCHMARK_FILE(types::TreeNode, Tree);
BENCHMARK_FILE(types::Organization, Nesting);
BENCHMARK_FILE(std::vector<tgbm::api::Update>, Updates_1);
BENCHMARK_FILE(std::vector<tgbm::api::Update>, Updates_5);
BENCHMARK_FILE(std::vector<tgbm::api::Update>, Updates_10);
BENCHMARK_FILE(std::vector<tgbm::api::Update>, Updates_50);
BENCHMARK_FILE(std::vector<tgbm::api::Update>, Updates_100);

BENCHMARK_FILE(types::TreeNode, RawTree)
BENCHMARK_FILE(std::vector<tgbm::api::Update>, RawUpdates_1);
BENCHMARK_FILE(std::vector<tgbm::api::Update>, RawUpdates_5);
BENCHMARK_FILE(std::vector<tgbm::api::Update>, RawUpdates_10);
BENCHMARK_FILE(std::vector<tgbm::api::Update>, RawUpdates_50);
BENCHMARK_FILE(std::vector<tgbm::api::Update>, RawUpdates_100);
