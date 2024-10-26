cmake_minimum_required(VERSION 3.10)

include(${CMAKE_SOURCE_DIR}/cmake/format.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/preprocess.cmake)

function(GenerateApiTypes)  
    set (TGBM_GENERATED_API false CACHE BOOL "")
    if (TGBM_GENERATED_API)
        return()
    endif()

    macro(handle_process_result result output error_output)
        if(NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Command execution failed with result: [${result}]. Output: [${output}]. Error output: [${error_output}]")
        endif()
    endmacro()

    set(TGBM_GEN ${CMAKE_SOURCE_DIR}/scripts/tgapi.py)
    set(TGBM_API_DIR "${CMAKE_SOURCE_DIR}/include/tgbm/api")
    message (STATUS "[TGBM] Download tgapi.html")
    execute_process(
        COMMAND ${TGBM_PYTHON} ${TGBM_GEN} --load_api=${TGBM_API_DIR}/tgapi.html
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )
    handle_process_result(${result} "${output}" "${error_output}")

    message (STATUS "[TGBM] Generate types.txt")
    execute_process(
        COMMAND ${TGBM_PYTHON} ${TGBM_GEN} --apidir=${TGBM_API_DIR} --get_classes 
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )
    handle_process_result(${result} "${output}" "${error_output}")

    message (STATUS "[TGBM] Generate defs")
    execute_process(
        COMMAND ${TGBM_PYTHON} ${TGBM_GEN} --apidir=${TGBM_API_DIR} --generate_defs 
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )
    handle_process_result(${result} "${output}" "${error_output}")

    PreprocessApiTypes()

    message (STATUS "[TGBM] postprocessing")

    execute_process(
        COMMAND ${TGBM_PYTHON} ${TGBM_GEN} --apidir=${TGBM_API_DIR} --postprocess 
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )
    handle_process_result(${result} "${output}" "${error_output}")
    FormatApiTypes()
    set (TGBM_GENERATED_API true CACHE STRING "" FORCE)
endfunction(GenerateApiTypes)
