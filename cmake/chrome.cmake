include("${CMAKE_CURRENT_LIST_DIR}/depot_tools.cmake")
find_program(CCACHE_EXECUTABLE ccache)
if(Python3_EXECUTABLE)
  if(DOWNLOAD_CHROMIUM)
    get_filename_component(chromium_base_dir "${CHROMIUM_SOURCE_TREE}" DIRECTORY)
    if(NOT IS_DIRECTORY "${CHROMIUM_SOURCE_TREE}")
        file(MAKE_DIRECTORY "${chromium_base_dir}")
        execute_process(
            COMMAND "${Python3_EXECUTABLE}" "${DEPOT_TOOLS_FETCH_PY}" --nohooks chromium
            WORKING_DIRECTORY "${chromium_base_dir}"
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    set(chromium_build_dir "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}")
    if(NOT IS_DIRECTORY "${chromium_build_dir}")
        if(CCACHE_EXECUTABLE)
            set(args " cc_wrapper=\"ccache\" ")
        endif()
        if(CMAKE_BUILD_TYPE STREQUAL Release)
            set(args " ${args} is_debug=false ")
        elseif(CMAKE_BUILD_TYPE STREQUAL Debug)
            message(STATUS "Building Chromium also in debug mode. Note, this has performance implications.")
        else()
            message(FATAL_ERROR "Please choose either Debug or Release for CMAKE_BUILD_TYPE (was '${CMAKE_BUILD_TYPE}').")
        endif()
        execute_process(
            COMMAND "${Python3_EXECUTABLE}" "${DEPOT_TOOLS_GN_PY}" "--args=' ${args} '" "out/${CHROMIUM_PROFILE}"
            WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}"
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
  endif()
endif()
