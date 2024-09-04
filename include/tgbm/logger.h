#pragma once

#include <iostream>
#include <fmt/format.h>

#define LOG(FMT_STR, ...) \
  ::fmt::format_to(::std::ostreambuf_iterator(std::cout), FMT_STR "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERR(FMT_STR, ...) \
  ::fmt::format_to(::std::ostreambuf_iterator(std::cerr), FMT_STR "\n" __VA_OPT__(, ) __VA_ARGS__)

#ifndef NDEBUG
#define LOG_DEBUG(FMT_STR, ...) LOG(FMT_STR __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_DEBUG(FMT_STR, ...) (void)0
#endif
