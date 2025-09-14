cmake_minimum_required(VERSION 3.20)
project(tgbm) # telegram bot mother library

option(TGBM_ENABLE_BENCHS   "Set to ON to enable benchmarks" OFF)
option(TGBM_ENABLE_GEN      "Set to ON to generate api"      OFF)
option(TGBM_ENABLE_TESTING  "Set to ON to enable tests"      OFF)
option(TGBM_ENABLE_EXAMPLES "Set to ON to enable examples"   OFF)
option(TGBM_USE_CPM "Set to ON to enable use CPM for get dependencies" ON)

set (TGBM_CLANG_FORMAT "clang-format" CACHE STRING "Used only by api generator")
set (TGBM_PYTHON       "python"       CACHE STRING "Used only by api generator")

# if setted, all SSL connections print their keys to provided file.
# this allows debug SSL connection via Wireshark. Disabled by default.
# Do not use in production ofc
set(TGBM_SSL_KEYS_FILE "" CACHE FILEPATH "")

if (TGBM_SSL_KEYS_FILE)
  message (STATUS "[TGBM] TGBM_SSL_KEYS_FILE: ${TGBM_SSL_KEYS_FILE}")
endif()
message (STATUS "[TGBM] TGBM_ENABLE_BENCHS: ${TGBM_ENABLE_BENCHS}")
message (STATUS "[TGBM] TGBM_ENABLE_GEN: ${TGBM_ENABLE_GEN}")
message (STATUS "[TGBM] TGBM_ENABLE_TESTING: ${TGBM_ENABLE_TESTING}")
message (STATUS "[TGBM] TGBM_ENABLE_EXAMPLES: ${TGBM_ENABLE_EXAMPLES}")
message (STATUS "[TGBM] TGBM_USE_CPM: ${TGBM_USE_CPM}")

message (STATUS "[TGBM] TGBM_CLANG_FORMAT: ${TGBM_CLANG_FORMAT}")
message (STATUS "[TGBM] TGBM_PYTHON: ${TGBM_PYTHON}")

# also option:
# - TGBM_WINDOWS_VERSION
#     define to set minimal supported windows version in boost asio
#     0x0A00 (windows 10) by default if windows

# disable warnings for third party libs
if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
endif()

if (TGBM_USE_CPM)
  include (cmake/deps.cmake)
endif()

if (TGBM_ENABLE_GEN)
  include (cmake/generate.cmake)
  TGBM_GENERATE_API()
endif()

set(TGBM_SRC_LIST
    src/bot.cpp
    src/long_poll.cpp
    src/api/input_file.cpp
    src/telegram.cpp
    src/net/http_base.cpp
    src/utils/url_encoded.cpp
    )

add_library(tgbmlib ${TGBM_SRC_LIST})

target_include_directories(tgbmlib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(tgbmlib PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
    anyanylib
    kelcorolib
    zallib
    http2lib
    Boost::system
    Boost::asio
    Boost::pfr
    Boost::json
    fmt::fmt
    rapidjsonlib
)

if (WIN32)
    if (NOT DEFINED TGBM_WINDOWS_VERSION) 
        set(TGBM_WINDOWS_VERSION 0x0A00 CACHE STRING "windows version for boost asio")
    endif()
    target_compile_definitions(tgbmlib PUBLIC _WIN32_WINNT=${TGBM_WINDOWS_VERSION})
endif()

target_compile_definitions(tgbmlib PUBLIC OPENSSL_NO_HEARTBEATS)

set_target_properties(tgbmlib PROPERTIES
	CMAKE_CXX_EXTENSIONS OFF
	LINKER_LANGUAGE CXX
	CMAKE_CXX_STANDARD_REQUIRED ON
	CXX_STANDARD 20)

# tests

if (TGBM_ENABLE_TESTING)
  include(CTest)
  add_subdirectory(gtest)
endif()

if (TGBM_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

if (TGBM_ENABLE_BENCHS)
  add_subdirectory(benchmarks)
endif()

# disable warnings for third party libs

if (MSVC)
  set(TGBM_NOWARN_FLAG /w)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR 
        CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR 
        CMAKE_CXX_COMPILER_ID STREQUAL "Intel" OR 
        CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(TGBM_NOWARN_FLAG -w)
endif()
target_compile_options(boost_container PRIVATE ${TGBM_NOWARN_FLAG})
target_compile_options(ssl PRIVATE ${TGBM_NOWARN_FLAG})
target_compile_options(crypto PRIVATE ${TGBM_NOWARN_FLAG})

if (WIN32)
  if (NOT DEFINED TGBM_WINDOWS_VERSION) 
      set(TGBM_WINDOWS_VERSION 0x0A00 CACHE STRING "windows version for boost asio")
  endif()
  message (STATUS "[tgbm] TGBM_WINDOWS_VERSION: ${TGBM_WINDOWS_VERSION}")
  target_compile_definitions(tgbmlib PUBLIC _WIN32_WINNT=${TGBM_WINDOWS_VERSION})
endif()
