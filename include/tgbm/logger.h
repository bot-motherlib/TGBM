#pragma once

#include <iostream>
#include <format>

#define LOG(FMT_STR, ...) ::std::format_to(::std::ostreambuf_iterator(std::cout), FMT_STR "\n", __VA_ARGS__)
#define LOG_ERR(FMT_STR, ...) \
  ::std::format_to(::std::ostreambuf_iterator(std::cerr), FMT_STR "\n", __VA_ARGS__)
