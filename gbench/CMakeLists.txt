add_executable(${PROJECT_NAME}_gbenchs
    files.cpp
    bench_main.cpp
)

target_compile_options(${PROJECT_NAME}_gbenchs
    PUBLIC
    "-Wno-undefined-var-template"
    "-Wno-reorder-init-list"
)

target_link_libraries(${PROJECT_NAME}_gbenchs
    PUBLIC tgbmlib benchmark::benchmark
)
target_include_directories(
    ${PROJECT_NAME}_gbenchs PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${PROJECT_NAME}_gbenchs  PROPERTIES
    CMAKE_CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
    CMAKE_CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 20
)
