include("${CMAKE_CURRENT_LIST_DIR}/depot_tools.cmake")
find_program(CCACHE_EXECUTABLE ccache)
set(chromium_build_dir "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}")
if(Python3_EXECUTABLE)
  if(CCACHE_EXECUTABLE)
    set(gnargs " cc_wrapper=\"${CCACHE_EXECUTABLE}\" ")
  endif()
  if(CMAKE_BUILD_TYPE STREQUAL Release)
    set(gnargs " ${gnargs} is_debug=false dcheck_always_on=false ")
  elseif(CMAKE_BUILD_TYPE STREQUAL Debug)
    message(STATUS "Building Chromium also in debug mode. Note, this has performance implications.")
  else()
    message(FATAL_ERROR "Please choose either Debug or Release for CMAKE_BUILD_TYPE (was '${CMAKE_BUILD_TYPE}').")
  endif()
  if(DOWNLOAD_CHROMIUM)
    get_filename_component(chromium_base_dir "${CHROMIUM_SOURCE_TREE}" DIRECTORY)
    if(NOT IS_DIRECTORY "${CHROMIUM_SOURCE_TREE}")
        file(MAKE_DIRECTORY "${chromium_base_dir}")
        execute_process(
            COMMAND "${Python3_EXECUTABLE}" "${DEPOT_TOOLS_FETCH_PY}" --nohooks --no-history chromium
            WORKING_DIRECTORY "${chromium_base_dir}"
            COMMAND_ERROR_IS_FATAL ANY
        )
	message(WARNING "And now, gclient runhooks. ${Python3_EXECUTABLE} ${DEPOT_TOOLS_GCLIENT_PY} -j 1 runhooks")
        execute_process(
          COMMAND "${Python3_EXECUTABLE}" "${DEPOT_TOOLS_GCLIENT_PY}"  runhooks -j 1
          WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}"
          COMMAND_ERROR_IS_FATAL ANY
        )
	message(WARNING "runhooks done.")
    endif()
    if(NOT IS_DIRECTORY "${chromium_build_dir}")
        execute_process(
            COMMAND "${Python3_EXECUTABLE}" "${DEPOT_TOOLS_GN_PY}" gen "out/${CHROMIUM_PROFILE}" "--args= ${gnargs} "
            WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}"
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
  endif()
endif()
