cmake_minimum_required(VERSION 3.10)

function(PreprocessApiTypes)
    set(TGBM_API_DIR "${CMAKE_SOURCE_DIR}/include/tgbm/api")
    set(CPP_FILE  "${CMAKE_SOURCE_DIR}/src/api_types_cmp.cpp")
    file(REMOVE ${CPP_FILE})
    file(APPEND ${CPP_FILE} "#include <tgbm/api/types_all.hpp>\n")
    file(APPEND ${CPP_FILE} "namespace tgbm::api {\n")

    
    file(MAKE_DIRECTORY ${TGBM_API_DIR}/types)
    file(GLOB_RECURSE TGBM_API_TYPES_FILES "${TGBM_API_DIR}/def/*.def")
    foreach(CUR_FILE IN LISTS TGBM_API_TYPES_FILES)
        # NAME_WE == name without extension
        get_filename_component(FILENAME ${CUR_FILE} NAME_WE)
        get_filename_component(TEMPLATE ${CUR_FILE} DIRECTORY)
        get_filename_component(TEMPLATE ${TEMPLATE} NAME)
        message(STATUS "[TGBM] preprocessing: ${TGBM_API_DIR}/types/${FILENAME}.hpp")
    
        set(template_file "${TGBM_API_DIR}/templates/${TEMPLATE}.hpp")
        
        execute_process(
            COMMAND ${TGBM_CLANG}
                    ${template_file}
                    -E
                    -P
                    -I ${CMAKE_SOURCE_DIR}/include
                    -nostdinc
                    -DAPI_TYPE=${FILENAME}
                    -DAPI_TYPE_PATH="${TGBM_API_DIR}/def/${TEMPLATE}/${FILENAME}.def"
                    -o ${TGBM_API_DIR}/types/${FILENAME}.hpp
            WORKING_DIRECTORY ${TGBM_API_DIR}
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
endfunction(PreprocessApiTypes)


