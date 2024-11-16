#include "tgbm/utils/url_encoded.hpp"

#include <cstdio>
#include <random>
#include <string>
#include <thread>

#include <boost/functional/hash.hpp>

namespace tgbm {

static size_t generate_random_seed() noexcept {
  size_t seed = std::random_device{}();
  boost::hash_combine(seed, std::hash<std::thread::id>{}(std::this_thread::get_id()));
  boost::hash_combine(seed, std::chrono::high_resolution_clock::now().time_since_epoch().count());
  return seed;
}

std::string generate_multipart_boundary(size_t length) {
  constexpr std::string_view chars = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890+=()?_";
  std::string result;
  result.reserve(length);
  thread_local std::mt19937 gen(generate_random_seed());
  std::uniform_int_distribution<size_t> dist(0, chars.size() - 1);
  for (size_t i = 0; i < length; ++i)
    result += chars[dist(gen)];
  return result;
}

}  // namespace tgbm
