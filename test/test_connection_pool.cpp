
#include <tgbm/net/connection_pool.hpp>

#include <iostream>
#include <source_location>
#include <fmt/format.h>

#include <kelcoro/thread_pool.hpp>

static void log_err(std::source_location loc = std::source_location::current()) {
  fmt::format_to(std::ostreambuf_iterator(std::cerr), "{} failed, location: {}:{}:{}", loc.function_name(),
                 loc.file_name(), loc.line(), loc.column());
  std::cerr.flush();
}
#define error_if(Cond)             \
  if (static_cast<bool>((Cond))) { \
    log_err();                     \
    std::exit(1);                  \
  }
#define TEST(NAME) static void test_connection_pool_##NAME()
#define RUN(NAME) test_connection_pool_##NAME()

inline std::atomic_int i = 0;
dd::task<int> test_factory() {
  co_return i.fetch_add(1);
}

TEST(base) {
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

int main() {
  RUN(base);
  return 0;
}
