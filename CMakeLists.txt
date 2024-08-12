cmake_minimum_required(VERSION 3.30)
project(tgbm) # telegram bot mother library

option(TGBM_SANITIZE_THREAD "" OFF)
option(TGBM_SANITIZE "" OFF)
option(TGBM_ENABLE_TESTS "Set to ON to enable building of tests" OFF)
option(BUILD_SHARED_LIBS "Build tgbm shared/static library" OFF)

if (TGBM_SANITIZE_THREAD)
  if (TGBM_SANITIZE)
    message(FATAL "TGBM_SANITIZE and TGBM_SANITIZE_THREAD incompatible, use only one of them")
  endif()
endif()

set(TGBM_SRC_LIST
    src/Api.cpp
    src/Bot.cpp
    src/EventHandler.cpp
    src/TgException.cpp
    src/TgTypeParser.cpp
    src/net/BoostHttpOnlySslClient.cpp
    src/net/HttpParser.cpp
    src/net/TgLongPoll.cpp
    src/net/Url.cpp
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
    src/types/ReactionType.cpp)

# deps

add_compile_definitions(BOOST_ASIO_HAS_CO_AWAIT)

# TODO rm unit test framework
find_package(Boost 1.82 COMPONENTS system unit_test_framework REQUIRED)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

include(FetchContent)

FetchContent_Declare(
  kelcoro
  GIT_REPOSITORY https://github.com/kelbon/kelcoro
  GIT_TAG origin/main
)
FetchContent_MakeAvailable(kelcoro)

FetchContent_Declare(
  AnyAny
  GIT_REPOSITORY https://github.com/kelbon/AnyAny
  GIT_TAG origin/main
)
FetchContent_MakeAvailable(AnyAny)

add_library(tgbmlib ${TGBM_SRC_LIST})

target_include_directories(tgbmlib PUBLIC 
                           "${CMAKE_CURRENT_SOURCE_DIR}/include"
                           PRIVATE
                           ${OPENSSL_INCLUDE_DIR}
                           ${Boost_INCLUDE_DIR})

target_link_libraries(tgbmlib PUBLIC
    kelcorolib
    anyanylib
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
    OpenSSL::SSL OpenSSL::Crypto
    ${Boost_LIBRARIES})

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
endif()

if(BUILD_SHARED_LIBS)
    add_definitions(-DTGBM_DLL)
endif()
