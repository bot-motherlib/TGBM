#pragma once

#include <fmt/format.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>
#include <fmt/color.h>

#define TGBM_LOG(FMT_STR, ...) ::fmt::print(FMT_STR "\n" __VA_OPT__(, ) __VA_ARGS__)

#define TGBM_LOG_WARN(FMT_STR, ...) \
  ::fmt::print(fg(fmt::color::yellow), FMT_STR "\n" __VA_OPT__(, ) __VA_ARGS__)

#define TGBM_LOG_INFO(FMT_STR, ...) \
  ::fmt::print(fg(fmt::color::blue), FMT_STR "\n" __VA_OPT__(, ) __VA_ARGS__)

#define TGBM_LOG_DEBUG(FMT_STR, ...) \
  ::fmt::print(fg(fmt::color::green), FMT_STR "\n" __VA_OPT__(, ) __VA_ARGS__)

#define TGBM_LOG_ERROR(FMT_STR, ...) \
  ::fmt::print(fg(fmt::color::red), FMT_STR "\n" __VA_OPT__(, ) __VA_ARGS__)
