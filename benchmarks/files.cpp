#include <benchmark/benchmark.h>

#include "fuzzing.hpp"
#include "types.hpp"
#include <tgbm/api/types/all.hpp>

#include <tgbm/jsons/boostjson_dom_traits.hpp>
#include <tgbm/jsons/boostjson_sax_producer.hpp>
#include "boost_parse_handler.hpp"
#include "rapid_parse_handler.hpp"
#include <tgbm/json.hpp>
#include <tgbm/jsons/sax_parser.hpp>
#include "handler_parser/all.hpp"
#include <tgbm/jsons/dom_parser.hpp>

#include <tgbm/jsons/rapidjson_dom_traits.hpp>
#include <tgbm/jsons/errors.hpp>
#include <tgbm/jsons/stream_parser.hpp>

template <typename T>
void execute_handler_rapid(std::string_view json) {
  benchmark::DoNotOptimize(tgbm::json::rapid::parse_handler<T>(json));
}

template <typename T>
void execute_dom_rapid(std::string_view json) {
  rapidjson::Document document;
  document.Parse(json.data(), json.size());
  if (document.HasParseError()) {
    TGBM_JSON_PARSE_ERROR;
  }
  benchmark::DoNotOptimize(tgbm::from_json<T, rapidjson::GenericValue<rapidjson::UTF8<>>>(document));
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
  ::boost::json::value j = boost::json::parse(json);
  benchmark::DoNotOptimize(tgbm::from_json<T>(j));
}

template <typename T>
void execute_ignore_handler_boost(std::string_view json) {
  benchmark::DoNotOptimize(tgbm::json::boost::parse_ignore_handler(json));
}

template <typename T>
T parse_generator(std::string_view data) {
  T v;
  tgbm::byte_t bytes[4096];
  tgbm::json::stream_parser parser(v, bytes);
  parser.feed(data, true);
  return v;
}

template <typename T>
void execute_generator_boost(std::string_view json) {
  benchmark::DoNotOptimize(parse_generator<T>(json));
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
