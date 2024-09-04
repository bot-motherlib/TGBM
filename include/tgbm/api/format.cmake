cmake_minimum_required(VERSION 3.10)

if (NOT DEFINED TGBM_CLANG_FORMAT)
    set(TGBM_CLANG "clang-format")
endif()

file(GLOB_RECURSE TGBM_API_TYPES_FILES "${CMAKE_CURRENT_LIST_DIR}/def/*.def")
set(TGBM_INC_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
message(STATUS "start formating: ${CMAKE_CURRENT_LIST_DIR}/types/*.hpp")
foreach(FILE IN LISTS TGBM_API_TYPES_FILES)
    get_filename_component(FILENAME ${FILE} NAME_WE) 

    execute_process(
        COMMAND ${TGBM_CLANG_FORMAT} ${CMAKE_CURRENT_LIST_DIR}/types/${FILENAME}.hpp -i --style=file:${TGBM_INC_DIR}/../.clang-format
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "clang-format execution failed with error: ${error_output}")
    endif()
message(STATUS "end formating: ${CMAKE_CURRENT_LIST_DIR}/types/*.hpp")
endforeach()
