#include <gtest/gtest.h>

#include <tgbm/net/http11/connection_pool.hpp>

#include <iostream>
#include <source_location>

#include <fmt/format.h>

static void log_err(std::source_location loc = std::source_location::current()) {
  fmt::format_to(std::ostreambuf_iterator(std::cerr), "{} failed, location: {}:{}:{}", loc.function_name(),
                 loc.file_name(), loc.line(), loc.column());
  std::cerr.flush();
}
#define error_if(...) EXPECT_FALSE((__VA_ARGS__))

inline std::atomic_int i = 0;
dd::task<int> test_factory() {
  co_return i.fetch_add(1);
}

TEST(connection_pool, base) {
  using namespace tgbm;
  enum { MAX_POOL_SIZE = 10 };
  pool_t<int> pool(MAX_POOL_SIZE, [] { return test_factory(); });
  using handle_t = pool_t<int>::handle_t;
  std::vector<handle_t> handles;
  for (int i = 0; i < MAX_POOL_SIZE; ++i) {
    handles.push_back(pool.borrow().get());
    error_if(*handles.back().get() != i);
  }
  auto handle = pool.borrow().start_and_detach();
  handles.pop_back();  // return one (and not inserted waiteer goes into borrowed)
  handles.clear();     // return all borrowed handles
  pool.shutdown();
}
