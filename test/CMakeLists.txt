set(TGBM_TEST_PARTS connection_pool tools basic_api_types)
foreach(PART ${TGBM_TEST_PARTS})
  add_executable(${PART} "${CMAKE_CURRENT_SOURCE_DIR}/test_${PART}.cpp")
  target_link_libraries(${PART} PUBLIC tgbmlib)
  set_target_properties(${PART} PROPERTIES
	  CMAKE_CXX_EXTENSIONS OFF
	  LINKER_LANGUAGE CXX
	  CMAKE_CXX_STANDARD_REQUIRED ON
	  CXX_STANDARD 20
  )

  add_test(NAME test_${PART} COMMAND ${PART})
endforeach()
