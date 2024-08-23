cmake_minimum_required(VERSION 3.10)
#project(preprocess_api_types)

### this script preprocess .def files into .h using clang -E ###

file(GLOB TGBM_API_TYPES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/def/*.def")

#add_custom_target(preprocess_api_types ALL
#    COMMAND ${CMAKE_COMMAND} -E echo "Processing files in ${CMAKE_CURRENT_SOURCE_DIR}/def..."
#)
set(TGBM_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..") # <project root>/include
message(STATUS "INCLUDES ${TGBM_INC_DIR}")
foreach(FILE IN LISTS TGBM_API_TYPES_FILES)
    # NAME_WE == name without extension
    get_filename_component(FILENAME ${FILE} NAME_WE)
    message(STATUS "Handling ${FILE}")
    execute_process(
        COMMAND clang
                ${CMAKE_CURRENT_SOURCE_DIR}/def/api_type_file_template.h
                -E
                -P
                -I ${TGBM_INC_DIR}
                -nostdinc
                -DTGBM_GENERATION_IN_PROCESS
                -DAPI_TYPE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/def/${FILENAME}.def"
                -o ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}_TEMP.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )

    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}_TEMP.h FILE_CONTENT)
    file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}.h
            "#pragma once\n#include \"tgbm/api_types/common.h\"\n${FILE_CONTENT}")
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}_TEMP.h)
  
    # format
  
    execute_process(
        COMMAND clang-format ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}.h
                -i --style=file:${TGBM_INC_DIR}/../.clang-format
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )
    message(STATUS OUT ${output})
    message(STATUS ERR ${error_output})
endforeach()
