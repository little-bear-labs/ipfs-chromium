cmake_minimum_required(VERSION 3.24)
project(chromium-ipfs)

set(CHROMIUM_SOURCE_TREE "/mnt/big/lbl/code/chromium/src" CACHE PATH "Path to chromium/src")
set(CHROMIUM_PROFILE "Default" CACHE STRING "The profile of the current Chromium build.")
set(DEPOT_TOOLS_DIRECTORY "DETECT_FROM_PATH" CACHE PATH "Where to find depot_tools, in particular DEPOT_TOOLS_DIRECTORY/gn.py should exist")
set(CXX_VERSION "17" CACHE STRING "The numeric part (year % 100) of the version of the C++ standard to be used. Must be at least 17 (for C++17). Must be one CMake knows about.")

if(CXX_VERSION GREATER_EQUAL 17)
    message(STATUS "Building for C++${CXX_VERSION}")
else()
    message(FATAL_ERROR "Unsupported CXX_VERSION: ${CXX_VERSION}")
endif()
include(cmake/setup.cmake)

enable_testing()

add_subdirectory(library)

if(IS_DIRECTORY "${CHROMIUM_SOURCE_TREE}")
    add_subdirectory(component)
else()
    message(WARNING "CHROMIUM_SOURCE_TREE not pointing to source tree.")
endif()
