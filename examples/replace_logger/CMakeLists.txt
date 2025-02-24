cmake_minimum_required(VERSION 3.20)

project(replace_logger)

add_executable(replace_logger ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_include_directories(tgbmlib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(replace_logger PRIVATE tgbmlib)

set_target_properties(replace_logger PROPERTIES CXX_STANDARD 20)
