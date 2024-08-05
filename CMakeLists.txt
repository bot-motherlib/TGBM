cmake_minimum_required(VERSION 3.30)
project(tgbm) # telegram bot mother library

option(TGBM_ENABLE_TESTS "Set to ON to enable building of tests" OFF)
option(BUILD_SHARED_LIBS "Build tgbm shared/static library" OFF)

set(TGBM_SRC_LIST
    src/Api.cpp
    src/Bot.cpp
    src/EventHandler.cpp
    src/TgException.cpp
    src/TgTypeParser.cpp
    src/net/BoostHttpOnlySslClient.cpp
    src/net/CurlHttpClient.cpp
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

# TODO 
find_package(Boost 1.82 COMPONENTS system unit_test_framework REQUIRED)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(CURL 7.58.0)
if (CURL_FOUND)
    add_definitions(-DHAVE_CURL)
endif()

find_package(fmt CONFIG REQUIRED)

# TODO use cmake for finding boost (newer version?)
# TODO rm unit test framework

add_library(tgbmlib ${TGBM_SRC_LIST})

target_include_directories(tgbmlib PUBLIC 
                           "${CMAKE_CURRENT_SOURCE_DIR}/include"
                           PRIVATE
                           ${OPENSSL_INCLUDE_DIR}
                           ${CURL_INCLUDE_DIRS}
                           ${Boost_INCLUDE_DIR})

target_link_libraries(tgbmlib PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
    OpenSSL::SSL OpenSSL::Crypto
    ${Boost_LIBRARIES}
    CURL::libcurl
    fmt::fmt)

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
    add_definitions(-DTGBOT_DLL)
endif()
