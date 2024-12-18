include(cmake/get_cpm.cmake)

CPMAddPackage(
  NAME OpenSSL
  GITHUB_REPOSITORY janbar/openssl-cmake
  GIT_TAG 1.1.1w-20231130
  OPTIONS "WITH_APPS OFF"
  EXCLUDE_FROM_ALL YES
  SYSTEM OFF
)

set(BOOST_INCLUDE_LIBRARIES system asio pfr json)
CPMAddPackage(
  NAME Boost
  VERSION 1.87.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.tar.xz
  OPTIONS "BOOST_ENABLE_CMAKE ON"
)
unset(BOOST_INCLUDE_LIBRARIES)
find_package(Boost 1.87 COMPONENTS system asio pfr json boost_container REQUIRED)

CPMAddPackage("gh:fmtlib/fmt#11.0.2")

CPMAddPackage(
  NAME kelcoro
  GIT_REPOSITORY https://github.com/kelbon/kelcoro
  GIT_TAG origin/main
  OPTIONS "KELCORO_ENABLE_TESTING ${TGBM_ENABLE_TESTING}"
)

CPMAddPackage(
  NAME AnyAny
  GIT_REPOSITORY https://github.com/kelbon/AnyAny
  GIT_TAG origin/main
  OPTIONS "AA_ENABLE_TESTING ${TGBM_ENABLE_TESTING}"
)

CPMAddPackage(
  NAME rapidjson
  GIT_REPOSITORY https://github.com/kelbon/rapidjson-usable
  GIT_TAG        origin/master
)

CPMAddPackage(
  NAME HPACK
  GIT_REPOSITORY https://github.com/kelbon/HPACK
  GIT_TAG        origin/master
  OPTIONS "HPACK_ENABLE_TESTING ${TGBM_ENABLE_TESTING}"
)

if (TGBM_ENABLE_TESTING)
  CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG release-1.12.1
    VERSION 1.12.1
    OPTIONS "INSTALL_GTEST OFF"
  )
endif()

if (TGBM_ENABLE_BENCHS)
  CPMAddPackage(
    NAME benchmark
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
  NAME telegram_bot_api_html
  GIT_REPOSITORY https://github.com/kelbon/telegram_bot_api_html
  GIT_TAG v8.1 # last supported api
  DOWNLOAD_ONLY YES
)

if (NOT DEFINED telegram_bot_api_html_SOURCE_DIR)
  message(FATAL "[TGBM] cannot load telegram bot api")
endif()

set(TGBM_APIFILE ${telegram_bot_api_html_SOURCE_DIR}/telegram_bot_api.html)

message(STATUS "[TGBM] telegram bot api file: ${TGBM_APIFILE}")
