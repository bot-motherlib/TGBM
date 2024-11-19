cmake_minimum_required(VERSION 3.20)

project(echobot)

add_executable(echobot ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(echobot PRIVATE tgbmlib)

set_target_properties(echobot PROPERTIES CXX_STANDARD 20)
