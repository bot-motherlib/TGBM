cmake_minimum_required(VERSION 3.20)

project(inline_keyboard)

add_executable(inline_keyboard ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(inline_keyboard PRIVATE tgbmlib)

set_target_properties(inline_keyboard PROPERTIES CXX_STANDARD 20)
