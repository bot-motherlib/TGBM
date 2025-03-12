cmake_minimum_required(VERSION 3.20)
project(tgbm) # telegram bot mother library

option(TGBM_ENABLE_BENCHS   "Set to ON to enable benchmarks" OFF)
option(TGBM_ENABLE_GEN      "Set to ON to generate api"      OFF)
option(TGBM_ENABLE_TESTING  "Set to ON to enable tests"      OFF)
option(TGBM_ENABLE_EXAMPLES "Set to ON to enable examples"   OFF)

set (TGBM_CLANG_FORMAT "clang-format" CACHE STRING "Used only by api generator")
set (TGBM_PYTHON       "python"       CACHE STRING "Used only by api generator")

message (STATUS "[TGBM] TGBM_ENABLE_BENCHS: ${TGBM_ENABLE_BENCHS}")
message (STATUS "[TGBM] TGBM_ENABLE_GEN: ${TGBM_ENABLE_GEN}")
message (STATUS "[TGBM] TGBM_ENABLE_TESTING: ${TGBM_ENABLE_TESTING}")
message (STATUS "[TGBM] TGBM_ENABLE_EXAMPLES: ${TGBM_ENABLE_EXAMPLES}")

message (STATUS "[TGBM] TGBM_CLANG_FORMAT: ${TGBM_CLANG_FORMAT}")
message (STATUS "[TGBM] TGBM_PYTHON: ${TGBM_PYTHON}")

# also options:
# - TGBM_SSL_KEYS_FILE
#     define to path where ssl keys will be stored for debug using wireshark
#     disabled by default
# - TGBM_WINDOWS_VERSION
#     define to set minimal supported windows version in boost asio
#     0x0A00 (windows 10) by default if windows

# disable warnings for third party libs
if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
endif()

include (cmake/deps.cmake)

if (TGBM_ENABLE_GEN)
  include (cmake/generate.cmake)
  TGBM_GENERATE_API()
endif()

set(TGBM_SRC_LIST
    src/net/asio_transport.cpp
    src/net/asio_tls_transport.cpp
    src/net/http2/client.cpp
    src/bot.cpp
    src/long_poll.cpp
    src/api/input_file.cpp
    src/telegram.cpp
    src/net/http2/protocol.cpp
    src/net/http2/server.cpp
    src/net/tcp_connection.cpp
    src/net/http11/client.cpp
    src/net/http_client.cpp
    src/net/ssl_context.cpp
    src/utils/url_encoded.cpp
    src/net/transport_factory.cpp
    )

add_library(tgbmlib ${TGBM_SRC_LIST})

target_include_directories(tgbmlib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(tgbmlib PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
    ssl crypto
    anyanylib
    kelcorolib
    hpacklib
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
  add_subdirectory(test)
  add_subdirectory(gtest)
endif()

if (TGBM_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

if (TGBM_ENABLE_BENCHS)
  add_subdirectory(benchmarks)
endif()

if (DEFINED TGBM_SSL_KEYS_FILE)
  message (STATUS "[tgbm] TGBM_SSL_KEYS_FILE: ${TGBM_SSL_KEYS_FILE}")
  target_compile_definitions(tgbmlib PUBLIC TGBM_SSL_KEYS_FILE="${TGBM_SSL_KEYS_FILE}")
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
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # add what you need
    target_compile_options(tgbmlib PUBLIC -freport-bug)
endif()
