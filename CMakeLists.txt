cmake_minimum_required(VERSION 3.22)
include(cmake/version.cmake)
project(ipfs-chromium
  VERSION ${version}
  DESCRIPTION "An ipfs client library intended for use in browsers, and a Chromium component which uses it."
  HOMEPAGE_URL https://github.com/little-bear-labs/ipfs-chromium
  LANGUAGES CXX
  )
#The cache variables in all-caps are meant for you to customize the build.
#They are all here, so you don't need to hunt.
set(DOWNLOAD_CHROMIUM FALSE CACHE BOOL
    "Whether you prefer this cmake run to fetch its own private copy of Chromium stuff. Takes a very long time."
)
set(CHROMIUM_SOURCE_TREE "${CMAKE_CURRENT_BINARY_DIR}/chromium/src" CACHE PATH
    "Path to chromium/src. If DOWNLOAD_CHROMIUM=TRUE, and it does not exist, it will be created."
)
set(CHROMIUM_PROFILE "${CMAKE_BUILD_TYPE}" CACHE STRING "The profile of the current Chromium build.")
set(CXX_VERSION "17" CACHE STRING "The numeric part (year % 100) of the version of the C++ standard to be used. Must be at least 17 (for C++17). Must be one CMake knows about.")
if(DOWNLOAD_CHROMIUM)
    set(default_dt "${CMAKE_CURRENT_BINARY_DIR}/depot_tools")
else()
    set(default_dt "DETECT_FROM_PATH")
endif()
set(DEPOT_TOOLS_DIRECTORY "${default_dt}" CACHE PATH
    "Where to find depot_tools, e.g. DEPOT_TOOLS_DIRECTORY/gn.py should exist. Setting this to DETECT_FROM_PATH implies the directory is in your PATH and always will be."
)
set(TEST_BY_DEFAULT FALSE CACHE BOOL "Update unit tests as part of the 'all' default target.")
set(USE_DOXYGEN TRUE CACHE BOOL "If we should attempt to use Doxygen to generate documentation.")
#End of user-configuration cache variables.

if(CXX_VERSION GREATER_EQUAL 17)
    message(STATUS "Building for C++${CXX_VERSION}")
else()
    message(FATAL_ERROR "Unsupported CXX_VERSION: ${CXX_VERSION}")
endif()

include(cmake/chrome.cmake)
include(cmake/setup.cmake)

enable_testing()

add_subdirectory(library)

if(IS_DIRECTORY "${CHROMIUM_SOURCE_TREE}")
    add_subdirectory(component)
elseif(CHROMIUM_SOURCE_TREE AND DOWNLOAD_CHROMIUM)
    message(STATUS "In download mode.")
    add_subdirectory(component)
else()
    message(WARNING "CHROMIUM_SOURCE_TREE (${CHROMIUM_SOURCE_TREE}) not pointing to source tree.")
endif()

include(cmake/doxy.cmake)
include(cmake/marp.cmake)
if(doc_targets)
    add_custom_target(doc
      DEPENDS ${doc_targets}
      COMMENT "Generated documentation: ${doc_targets}"
    )
endif()
