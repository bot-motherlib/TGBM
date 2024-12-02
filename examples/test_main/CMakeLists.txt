cmake_minimum_required(VERSION 3.20)

project(test_main)

add_executable(test_main ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(test_main PRIVATE tgbmlib)

set_target_properties(test_main PROPERTIES CXX_STANDARD 20)
