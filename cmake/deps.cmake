include(cmake/get_cpm.cmake)

set(BOOST_INCLUDE_LIBRARIES system asio pfr json)
CPMAddPackage(
  NAME BOOST
  VERSION 1.87.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.tar.xz
  OPTIONS "BOOST_ENABLE_CMAKE ON"
)
unset(BOOST_INCLUDE_LIBRARIES)

CPMAddPackage("gh:fmtlib/fmt#11.2.0")

CPMAddPackage(
  NAME KELCORO
  GIT_REPOSITORY https://github.com/kelbon/kelcoro
  GIT_TAG v1.2.0
  OPTIONS "KELCORO_ENABLE_TESTING OFF"
)

CPMAddPackage(
  NAME ANYANY
  GIT_REPOSITORY https://github.com/kelbon/AnyAny
  GIT_TAG v1.1.0
  OPTIONS "AA_ENABLE_TESTING ${TGBM_ENABLE_TESTING}"
)

CPMAddPackage(
  NAME RAPIDJSON
  GIT_REPOSITORY https://github.com/kelbon/rapidjson-usable
  GIT_TAG        0931914e89b021fcf521d3bb39b501b96fbeffd2
)

if (TGBM_ENABLE_TESTING)
  CPMAddPackage(
    NAME GOOGLETEST
    GITHUB_REPOSITORY google/googletest
    GIT_TAG release-1.12.1
    VERSION 1.12.1
    OPTIONS "INSTALL_GTEST OFF"
  )
endif()

if (TGBM_ENABLE_BENCHS)
  CPMAddPackage(
    NAME BENCHMARK
    GITHUB_REPOSITORY google/benchmark
    GIT_TAG v1.9.0
    OPTIONS "BENCHMARK_ENABLE_TESTING Off"
            "BENCHMARK_ENABLE_WERROR OFF"
            "BENCHMARK_FORCE_WERROR OFF"
            "BENCHMARK_INSTALL_DOCS OFF"
  )
endif()

find_package(Threads REQUIRED)

CPMAddPackage(
  NAME TELEGRAM_BOT_API_HTML
  GIT_REPOSITORY https://github.com/kelbon/telegram_bot_api_html
  GIT_TAG v9.2 # last supported api
  DOWNLOAD_ONLY YES
)

if (NOT DEFINED TELEGRAM_BOT_API_HTML_SOURCE_DIR)
  message(FATAL "[TGBM] cannot load telegram bot api")
endif()

CPMAddPackage(
  NAME HTTP2
  GIT_REPOSITORY https://github.com/kelbon/http2
  GIT_TAG origin/main
  OPTIONS "KELHTTP2_USE_CPM ON"
          "KELHTTP2_DEBUG_SSL_KEYS_FILE ${TGBM_SSL_KEYS_FILE}"
)

set(TGBM_APIFILE ${TELEGRAM_BOT_API_HTML_SOURCE_DIR}/telegram_bot_api.html)

message(STATUS "[TGBM] telegram bot api file: ${TGBM_APIFILE}")
