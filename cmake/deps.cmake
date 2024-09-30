include(cmake/CPM.cmake)

CPMAddPackage(
  NAME OpenSSL
  GITHUB_REPOSITORY janbar/openssl-cmake
  GIT_TAG 1.1.1w-20231130
  OPTIONS "WITH_APPS OFF"
  EXCLUDE_FROM_ALL YES
  SYSTEM OFF
)

set(BOOST_INCLUDE_LIBRARIES system asio property_tree pfr json)
CPMAddPackage(
  NAME Boost
  VERSION 1.84.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  OPTIONS "BOOST_ENABLE_CMAKE ON"
)
unset(BOOST_INCLUDE_LIBRARIES)
find_package(Boost 1.84 COMPONENTS system asio property_tree pfr json REQUIRED)

CPMAddPackage(
  NAME ZLIB
  Version 1.3.1
  GIT_REPOSITORY https://github.com/madler/zlib
  GIT_TAG v1.3.1
  OPTIONS "ZLIB_BUILD_EXAMPLES OFF"
)
find_package(ZLIB REQUIRED)

CPMAddPackage("gh:fmtlib/fmt#10.2.0")

CPMAddPackage(
  NAME kelcoro
  GIT_REPOSITORY https://github.com/kelbon/kelcoro
  GIT_TAG origin/main
)

CPMAddPackage(
  NAME AnyAny
  GIT_REPOSITORY https://github.com/kelbon/AnyAny
  GIT_TAG origin/main
)

CPMAddPackage(
  NAME rapidjson
  GIT_REPOSITORY https://github.com/kelbon/rapidjson-usable
  GIT_TAG        origin/master
)

if (TGBM_ENABLE_TESTS)
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
    VERSION 1.7.1
    OPTIONS "BENCHMARK_ENABLE_TESTING Off"
  )
endif()

find_package(Threads REQUIRED)
