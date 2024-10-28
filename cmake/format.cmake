cmake_minimum_required(VERSION 3.10)

# accepts dir, for example 'my/dir' and executes clang-format on each file in it
macro(TGBM_FORMAT_DIR DIRPATH)
    message(INFO "[TGBM] formatting ${TGBM_FORMAT_DIR}")
    execute_process(
        COMMAND ${TGBM_CLANG_FORMAT} -i --style=file:${CMAKE_SOURCE_DIR}/.clang-format ./*
        WORKING_DIRECTORY ${DIRPATH}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )
    if(NOT result EQUAL 0)
        message(ERROR "clang-format execution failed with error: ${error_output}")
    endif()
endmacro(TGBM_FORMAT_DIR)
