cmake_minimum_required(VERSION 3.10)
#project(preprocess_api_types)

### this script preprocess .def files into .h using clang -E ###

if (NOT DEFINED TGBM_CLANG)
    set(TGBM_CLANG "clang")
endif()

file(GLOB_RECURSE TGBM_API_TYPES_FILES "${CMAKE_CURRENT_LIST_DIR}/def/*.def")
set(TGBM_INC_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
set(CPP_FILE  "${CMAKE_CURRENT_LIST_DIR}/../../../src/api.cpp")
file(REMOVE ${CPP_FILE})
file(APPEND ${CPP_FILE} "#include <tgbm/api/types_all.hpp>\n")
file(APPEND ${CPP_FILE} "namespace tgbm::api {\n")

foreach(CUR_FILE IN LISTS TGBM_API_TYPES_FILES)
    message(STATUS "preprocessing: ${CMAKE_CURRENT_LIST_DIR}/types/${FILENAME}.hpp")
    # NAME_WE == name without extension
    get_filename_component(FILENAME ${CUR_FILE} NAME_WE)
    get_filename_component(TEMPLATE ${CUR_FILE} DIRECTORY)
    get_filename_component(TEMPLATE ${TEMPLATE} NAME)

    set(template_file "${CMAKE_CURRENT_LIST_DIR}/templates/${TEMPLATE}.h")
    
    execute_process(
        COMMAND ${TGBM_CLANG}
                ${template_file}
                -E
                -P
                -I ${TGBM_INC_DIR}
                -nostdinc
                -DAPI_TYPE=${FILENAME}
                -DAPI_TYPE_PATH="${CMAKE_CURRENT_LIST_DIR}/def/${TEMPLATE}/${FILENAME}.def"
                -o ${CMAKE_CURRENT_LIST_DIR}/types/${FILENAME}.hpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
    )
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "generation failed with error: ${error_output}")
    endif()

    set(LINE "\tstd::strong_ordering ${FILENAME}::operator<=>(const ${FILENAME}&) const = default\; \n")
    file(APPEND ${CPP_FILE} ${LINE})
    set(LINE "\tbool ${FILENAME}::operator==(const ${FILENAME}&) const = default\; \n")
    file(APPEND ${CPP_FILE} ${LINE})

endforeach()

file(APPEND ${CPP_FILE} "}")
