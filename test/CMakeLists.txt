add_executable(tgbm_fake_server ${CMAKE_CURRENT_SOURCE_DIR}/fake_server.cpp)
target_link_libraries(tgbm_fake_server PUBLIC tgbmlib)

set_target_properties(tgbm_fake_server PROPERTIES
  CMAKE_CXX_EXTENSIONS OFF
  LINKER_LANGUAGE CXX
  CMAKE_CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 20)
