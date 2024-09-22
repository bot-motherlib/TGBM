#pragma once

#include "kelcoro/noexport/macro.hpp"

#ifdef __clang__
#define TGBM_TRIVIAL_ABI [[clang::trivial_abi]]
#else
#define TGBM_TRIVIAL_ABI
#endif

#ifndef TGBM_ENABLE_DEBUG
#define TGBM_ENABLE_DEBUG NDEBUG
#endif

#ifdef TGBM_ENABLE_DEBUG
// clang-format off
#define TGBM_ON_DEBUG(...) do { [&]() __VA_ARGS__(); } while (false)
#define TGBM_DEBUG_FIELD(name, ...) __VA_ARGS__ name

#else

#define TGBM_ON_DEBUG(...) do {} while (false)
#define TGBM_DEBUG_FIELD(name, ...)
// clang-format on

#endif
namespace tgbm {
// forbids move for type without breaking 'is_aggregate'
struct pin {
  pin() = default;
  pin(pin&&) = delete;
  void operator=(pin&&) = delete;
};

}  // namespace tgbm

// must be used as field in type, makes it unmovable without breaking is_aggregate
#define TGBM_PIN KELCORO_NO_UNIQUE_ADDRESS ::tgbm::pin _pin_;
