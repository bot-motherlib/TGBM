cmake_minimum_required(VERSION 3.10)

function(FormatApiTypes)
    set(TGBM_API_DIR "${CMAKE_SOURCE_DIR}/include/tgbm/api")
    file(GLOB_RECURSE TGBM_API_TYPES_FILES "${TGBM_API_DIR}/def/*.def")
    foreach(FILE IN LISTS TGBM_API_TYPES_FILES)
        get_filename_component(FILENAME ${FILE} NAME_WE) 
        message(STATUS "start formating: ${TGBM_API_DIR}/types/${FILENAME}.hpp")
        execute_process(
            COMMAND ${TGBM_CLANG_FORMAT} ${TGBM_API_DIR}/types/${FILENAME}.hpp -i --style=file:${CMAKE_SOURCE_DIR}/.clang-format
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE result
            OUTPUT_VARIABLE output
            ERROR_VARIABLE error_output
        )
        if(NOT result EQUAL 0)
            message(FATAL_ERROR "clang-format execution failed with error: ${error_output}")
        endif()
    endforeach()
endfunction(FormatApiTypes)



