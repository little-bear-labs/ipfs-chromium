cmake_minimum_required(VERSION 3.24)
project(chromium-ipfs)

set(CHROMIUM_SOURCE_TREE "/mnt/big/lbl/code/chromium/src" CACHE STRING "Path to chromium/src")
set(CHROMIUM_OUT_DIR "${CHROMIUM_SOURCE_TREE}/out/Default" CACHE STRING "Path to chromium/src")
include(cmake/fromoutside.cmake)

file(
    GLOB
        sources
    CONFIGURE_DEPENDS
        component/*.cc
)

add_library(ipfs_component
    ${sources}
)
target_include_directories(ipfs_component
    SYSTEM
    BEFORE
    PUBLIC
        ${chromium_external_inc_dirs}
)
target_compile_options(ipfs_component
    PUBLIC
        #TODO(maybe)
        -nostdinc++
        -fno-rtti
)

foreach(source ${sources})
#    message(WARNING "source=${source}")
    get_filename_component(basename "${source}" NAME )
    set(gn_sources "${gn_sources}  \"${basename}\",\n  ")
endforeach()
configure_file(
        BUILD.gn.in
        ${CMAKE_CURRENT_SOURCE_DIR}/component/BUILD.gn
    @ONLY
)

add_executable(dummy dummy_main.cpp )
target_link_libraries(dummy
    PRIVATE
        ipfs_component
        ${chromium_shared_objects}
        ${chromium_static_archives}
)
