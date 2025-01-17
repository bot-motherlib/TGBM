cmake_minimum_required(VERSION 3.10)

function(TGBM_GENERATE_API)
    set (TGBM_API_GENERATED false CACHE BOOL "")
    if (TGBM_API_GENERATED)
        return()
    endif()

    # formats all files in directory
    macro(TGBM_FORMAT_DIR DIRPATH)
        message(STATUS "[TGBM] formatting ${DIRPATH}\n")
        execute_process(
            COMMAND ${TGBM_CLANG_FORMAT} -i --style=file:${CMAKE_SOURCE_DIR}/.clang-format ./*
            WORKING_DIRECTORY ${DIRPATH}
            RESULT_VARIABLE result
            OUTPUT_VARIABLE output
            ERROR_VARIABLE error_output
        )
    endmacro(TGBM_FORMAT_DIR)

    # generate types into tgbm/api/types

    execute_process(
        COMMAND ${TGBM_PYTHON} ${CMAKE_SOURCE_DIR}/scripts/tgapi_types.py --outdir=${CMAKE_SOURCE_DIR}/include/tgbm/api/types --apifile=${TGBM_APIFILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )
    if(NOT ${result} EQUAL 0)
        message(FATAL_ERROR "fail while generating types: [${result}]. Output: [${output}]. Error output: [${error_output}]")
    endif()

    message(STATUS  ${output})

    # generate methods into tgbm/api/methods

    execute_process(
        COMMAND ${TGBM_PYTHON} ${CMAKE_SOURCE_DIR}/scripts/tgapi_methods.py --outdir=${CMAKE_SOURCE_DIR}/include/tgbm/api/methods --apifile=${TGBM_APIFILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )
    if(NOT ${result} EQUAL 0)
        message(FATAL_ERROR "fail while generating methods: Out: ${output}, Error output: [${error_output}]")
    endif()

    message(STATUS ${output})
    TGBM_FORMAT_DIR("${CMAKE_SOURCE_DIR}/include/tgbm/api/types")
    TGBM_FORMAT_DIR("${CMAKE_SOURCE_DIR}/include/tgbm/api/methods")
    # src telegram.cpp telegram.hpp
    TGBM_FORMAT_DIR("${CMAKE_SOURCE_DIR}/include/tgbm/api")
    TGBM_FORMAT_DIR("${CMAKE_SOURCE_DIR}/src")

    set (TGBM_API_GENERATED true CACHE STRING "" FORCE)
endfunction(TGBM_GENERATE_API)
