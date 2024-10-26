cmake_minimum_required(VERSION 3.20)
project(tgbm) # telegram bot mother library

option(TGBM_ENABLE_BENCHS   "Set to ON to enable building of benchs" OFF)
option(TGBM_ENABLE_GEN      "Set to ON to generate api"              OFF)
option(TGBM_ENABLE_TESTS    "Set to ON to enable building of tests"  OFF)
option(TGBM_SANITIZE        ""  OFF)
option(TGBM_SANITIZE_THREAD ""  OFF)

set (TGBM_CLANG        "clang"        CACHE STRING "Used only by api generator")
set (TGBM_CLANG_FORMAT "clang-format" CACHE STRING "Used only by api generator")
set (TGBM_PYTHON       "python"       CACHE STRING "Used only by api generator")

message (STATUS "[tgbm] TGBM_ENABLE_BENCHS: ${TGBM_ENABLE_BENCHS}")
message (STATUS "[tgbm] TGBM_ENABLE_GEN: ${TGBM_ENABLE_GEN}")
message (STATUS "[tgbm] TGBM_ENABLE_TESTS: ${TGBM_ENABLE_TESTS}")
message (STATUS "[tgbm] TGBM_SANITIZE: ${TGBM_SANITIZE}")
message (STATUS "[tgbm] TGBM_SANITIZE_THREAD: ${TGBM_SANITIZE_THREAD}")

message (STATUS "[tgbm] TGBM_CLANG: ${TGBM_CLANG}")
message (STATUS "[tgbm] TGBM_CLANG_FORMAT: ${TGBM_CLANG_FORMAT}")
message (STATUS "[tgbm] TGBM_PYTHON: ${TGBM_PYTHON}")

# also options:
# - TGBM_SSL_KEYS_FILE
#     define to path where ssl keys will be stored for debug using wireshark
#     disabled by default
# - TGBM_WINDOWS_VERSION
#     define to set minimal supported windows version in boost asio
#     0x0A00 (windows 10) by default if windows

if (TGBM_SANITIZE_THREAD)
  if (TGBM_SANITIZE)
    message(FATAL "TGBM_SANITIZE and TGBM_SANITIZE_THREAD incompatible, use only one of them")
  endif()
endif()

# disable warnings for third party libs
if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
endif()

include (cmake/deps.cmake)

if (TGBM_ENABLE_GEN)
  include (cmake/generate.cmake)
  GenerateApiTypes()
endif()

set(TGBM_SRC_LIST
    src/net/http2_client.cpp
    src/Api.cpp
    # src/api_types_cmp.cpp
    src/Bot.cpp
    src/EventHandler.cpp
    src/TgTypeParser.cpp
    src/net/http2/protocol.cpp
    src/net/http2_server.cpp
    src/net/tcp_connection.cpp
    src/net/http11_client.cpp
    src/net/http_client.cpp
    src/net/HttpParser.cpp
    src/net/long_poll.cpp
    src/net/ssl_context.cpp
    src/tools/FileTools.cpp
    src/tools/StringTools.cpp
    src/types/BotCommandScope.cpp
    src/types/ChatBoostSource.cpp
    src/types/ChatMember.cpp
    src/types/InlineQueryResult.cpp
    src/types/InputFile.cpp
    src/types/InputMedia.cpp 
    src/types/InputMessageContent.cpp
    src/types/MenuButton.cpp
    src/types/MessageOrigin.cpp
    src/types/PassportElementError.cpp
    src/types/ReactionType.cpp
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
    Boost::property_tree
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

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if (TGBM_SANITIZE_THREAD)
    set(TGBM_SANITIZER_FLAGS "-fsanitize=thread")
  endif()
  if (TGBM_SANITIZE)
    set(TGBM_SANITIZER_FLAGS "-fsanitize=address,undefined")
    if (NOT WIN32)
      set(TGBM_SANITIZER_FLAGS "${TGBM_SANITIZER_FLAGS},memory")
    endif()
  endif()
  add_compile_options(${TGBM_SANITIZER_FLAGS})
  add_link_options(${TGBM_SANITIZER_FLAGS})
endif()

set_target_properties(tgbmlib PROPERTIES
	CMAKE_CXX_EXTENSIONS OFF
	LINKER_LANGUAGE CXX
	CMAKE_CXX_STANDARD_REQUIRED ON
	CXX_STANDARD 20)

# tests

if (TGBM_ENABLE_TESTS)
    include(CTest)
    add_subdirectory(test)
    add_subdirectory(gtest)
endif()

if (TGBM_ENABLE_BENCHS)
    add_subdirectory(gbench)
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
