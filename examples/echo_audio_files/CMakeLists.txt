cmake_minimum_required(VERSION 3.20)

project(echo_audio_files)

add_executable(echo_audio_files ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(echo_audio_files PRIVATE tgbmlib)

set_target_properties(echo_audio_files PROPERTIES CXX_STANDARD 20)
