add_executable(tgbm_fake_server ${CMAKE_CURRENT_SOURCE_DIR}/fake_server.cpp)
target_link_libraries(tgbm_fake_server PUBLIC tgbmlib)

set_target_properties(tgbm_fake_server PROPERTIES
  CMAKE_CXX_EXTENSIONS OFF
  LINKER_LANGUAGE CXX
  CMAKE_CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 20)

add_executable(tgbm_client_server_http2_smoke ${CMAKE_CURRENT_SOURCE_DIR}/server_client_smoke.cpp)
target_link_libraries(tgbm_client_server_http2_smoke PUBLIC tgbmlib)

set_target_properties(tgbm_client_server_http2_smoke PROPERTIES
  CMAKE_CXX_EXTENSIONS OFF
  LINKER_LANGUAGE CXX
  CMAKE_CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 20)

add_test(tgbm_http2_client_server tgbm_client_server_http2_smoke)

target_compile_definitions(tgbm_client_server_http2_smoke PRIVATE
  TEST_SERVER_CERT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test_server.crt"
  TEST_SERVER_KEY_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test_server.key"
)

add_executable(tgbm_http2_notls_client ${CMAKE_CURRENT_SOURCE_DIR}/client_smoke_no_tls.cpp)
target_link_libraries(tgbm_http2_notls_client PUBLIC tgbmlib)

set_target_properties(tgbm_http2_notls_client PROPERTIES
  CXX_STANDARD 20)

add_test(tgbm_http2_notls_client tgbm_http2_notls_client)
