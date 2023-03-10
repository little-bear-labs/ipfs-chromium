include("${CMAKE_CURRENT_LIST_DIR}/python.cmake")
include(FetchContent)
if(Python3_EXECUTABLE)
  if("${DEPOT_TOOLS_DIRECTORY}" STREQUAL "DETECT_FROM_PATH")
      find_program(
          tool_path
          NAMES autoninja.py autoninja autoninja.bat gn.py gn gn.bat #Not all of these may be marked as executable, which is OK but makes them unfindable.
          NO_CACHE
      )
      if(tool_path)
        #Note that the result of this call is not cached.
        get_filename_component(
            DEPOT_TOOLS_DIRECTORY
            "${tool_path}"
            DIRECTORY
        )
      endif()
  elseif(IS_DIRECTORY DEPOT_TOOLS_DIRECTORY)
    message(STATUS "DEPOT_TOOLS_DIRECTORY set to '${DEPOT_TOOLS_DIRECTORY}'.")
  elseif(DOWNLOAD_CHROMIUM)
    message(STATUS "Fetch depot_tools")
    FetchContent_Declare(
        depot_tools
        SOURCE_DIR "${DEPOT_TOOLS_DIRECTORY}"
        GIT_REPOSITORY https://chromium.googlesource.com/chromium/tools/depot_tools.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(depot_tools)
  endif()
  if(EXISTS "${DEPOT_TOOLS_DIRECTORY}/ninja.py")
      message(STATUS "depot_tools found at ${DEPOT_TOOLS_DIRECTORY}")
      set(DEPOT_TOOLS_NINJA_PY "${DEPOT_TOOLS_DIRECTORY}/ninja.py")
  else()
      message(WARNING "DEPOT_TOOLS_DIRECTORY(${DEPOT_TOOLS_DIRECTORY}) not properly leading to a ninja.py - will not be doing a proper build of the component inside the Chromium tree.")
  endif()
  if(EXISTS "${DEPOT_TOOLS_DIRECTORY}/gn.py")
      set(DEPOT_TOOLS_GN_PY "${DEPOT_TOOLS_DIRECTORY}/gn.py")
  else()
      message(WARNING "DEPOT_TOOLS_DIRECTORY(${DEPOT_TOOLS_DIRECTORY}) not properly leading to a gn.py.")
  endif()
  if(EXISTS "${DEPOT_TOOLS_DIRECTORY}/fetch.py")
      set(DEPOT_TOOLS_FETCH_PY "${DEPOT_TOOLS_DIRECTORY}/fetch.py")
  else()
      message(WARNING "DEPOT_TOOLS_DIRECTORY(${DEPOT_TOOLS_DIRECTORY}) not properly leading to a fetch.py.")
  endif()
  set(ENV{PATH} "${DEPOT_TOOLS_DIRECTORY}:$ENV{PATH}")#TODO - do we need to do ; for Win?
else()
  message(WARNING "No python, no depot_tools.")
endif()
