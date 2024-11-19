cmake_minimum_required(VERSION 3.20)

project(photo)

add_executable(photo ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(photo PRIVATE tgbmlib)

set_target_properties(photo PROPERTIES CXX_STANDARD 20)
target_compile_definitions(photo PRIVATE EXAMPLE_PHOTO_PATH="${CMAKE_CURRENT_SOURCE_DIR}/example_cat.jpg")
