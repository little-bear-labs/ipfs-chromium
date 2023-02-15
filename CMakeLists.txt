cmake_minimum_required(VERSION 3.24)
project(chromium-ipfs)

set(CHROMIUM_SOURCE_TREE "/mnt/big/lbl/code/chromium/src" CACHE PATH "Path to chromium/src")
set(CHROMIUM_PROFILE "Default" CACHE STRING "The profile of the current Chromium build.")
set(DEPOT_TOOLS_DIRECTORY "DETECT_FROM_PATH" CACHE PATH "Where to find depot_tools, in particular DEPOT_TOOLS_DIRECTORY/gn.py should exist")

include(cmake/setup.cmake)

add_subdirectory(library)
add_executable(simple simple_main.cpp)
target_compile_features(simple
    PUBLIC
    cxx_std_17
)
find_package(absl)
target_link_libraries(simple
    PUBLIC
        ipfs_client
        absl::statusor
)
if(IS_DIRECTORY "${CHROMIUM_SOURCE_TREE}")
    add_subdirectory(component)
else()
    message(WARNING "CHROMIUM_SOURCE_TREE not pointing to source tree.")
endif()
