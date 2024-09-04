#pragma once

#include "kelcoro/noexport/macro.hpp"

#ifdef __clang__
#define TGBM_TRIVIAL_ABI [[clang::trivial_abi]]
#else
#define TGBM_TRIVIAL_ABI
#endif

#ifndef TGBM_DEBUG
#define TGBM_DEBUG NDEBUG
#endif

#ifdef TGBM_DEBUG
// clang-format off
#define TGBM_ON_DEBUG(...) do { [&]() __VA_ARGS__(); } while (false)
#define DEBUG_FIELD(type, field) type field

#else

#define TGBM_ON_DEBUG(...) do {} while (false)
#define DEBUG_FIELD(type, field)
// clang-format on

#endif
