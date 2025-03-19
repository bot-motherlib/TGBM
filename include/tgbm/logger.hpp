#pragma once
/*
  simplest replacable logger
  To replace logger user should create tgbm_replace/logger.hpp file
  and add ../tgbm_replace into include dirs
*/

#include <tgbm/utils/macro.hpp>
#if __has_include(<tgbm_replace/logger.hpp>)
  #include <tgbm_replace/logger.hpp>
  #ifndef TGBM_LOG
    #error <tgbm_replace/logger.hpp> must define TGBM_LOG(FMT_STR, ...ARGS)
  #endif
#else

  #include <fmt/format.h>
  #include <fmt/core.h>
  #include <fmt/ranges.h>
  #include <fmt/chrono.h>

namespace tgbm::noexport {

template <typename... Args>
TGBM_GCC_WORKAROUND void do_log(fmt::format_string<Args...> fmtstr, Args&&... args) {
  try {
    ::fmt::println(stdout, fmtstr, std::forward<Args>(args)...);
  } catch (...) {
    ::fmt::println(stdout, "FMT_ERR");
  }
}
}  // namespace tgbm::noexport

// try catch is workaround fmt bug with "invalid" utf (exception on valid boost asio error strings)
  #define TGBM_LOG(FMT_STR, ...) ::tgbm::noexport::do_log(FMT_STR __VA_OPT__(, ) __VA_ARGS__)

#endif

#ifndef TGBM_LOG_INFO
  #define TGBM_LOG_INFO(FMT_STR, ...) TGBM_LOG("[INFO] " FMT_STR __VA_OPT__(, ) __VA_ARGS__)
#endif

#ifndef TGBM_LOG_ERROR
  #define TGBM_LOG_ERROR(FMT_STR, ...) TGBM_LOG("[ERROR] " FMT_STR __VA_OPT__(, ) __VA_ARGS__)
#endif

#ifndef TGBM_LOG_WARN
  #define TGBM_LOG_WARN(FMT_STR, ...) TGBM_LOG("[WARN] " FMT_STR __VA_OPT__(, ) __VA_ARGS__)
#endif

#ifndef TGBM_LOG_DEBUG
  #ifndef NDEBUG
    #define TGBM_LOG_DEBUG(FMT_STR, ...) TGBM_LOG("[DEBUG] " FMT_STR __VA_OPT__(, ) __VA_ARGS__)
  #else
    #define TGBM_LOG_DEBUG(FMT_STR, ...) (void)0
  #endif
#endif

#ifndef TGBM_HTTP2_LOG
  #define TGBM_HTTP2_LOG(TYPE, STR, ...) TGBM_LOG_##TYPE("[HTTP2] " STR __VA_OPT__(, ) __VA_ARGS__)
#endif
