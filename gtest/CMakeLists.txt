include(GoogleTest)

add_executable(${PROJECT_NAME}_gtests 
    api/basic_types.cpp
    parsers/test_basic.cpp
    parsers/test_tree.cpp
    parsers/test_optional.cpp
    parsers/test_object.cpp
    parsers/test_array.cpp
    parsers/test_oneof.cpp
    parsers/test_oneof_field.cpp
    parsers/test_stream.cpp
    http/test_body.cpp
    http/connection_pool.cpp
    utils.cpp
)

target_compile_options(${PROJECT_NAME}_gtests
    PUBLIC
    "-Wno-undefined-var-template"
    "-Wno-reorder-init-list"
    "-g"
)

target_link_libraries(${PROJECT_NAME}_gtests 
    PUBLIC tgbmlib
    GTest::gtest
    GTest::gtest_main
)
target_include_directories(
    ${PROJECT_NAME}_gtests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${PROJECT_NAME}_gtests  PROPERTIES
    CMAKE_CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
    CMAKE_CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 20
)

add_test(TGBM_gtests tgbm_gtests)
