cmake_minimum_required(VERSION 3.24)
project(chromium-ipfs)

set(CHROMIUM_SOURCE_TREE "/mnt/big/lbl/code/chromium/src" CACHE STRING "Path to chromium/src")
set(CHROMIUM_PROFILE "Default" CACHE STRING "The profile of the current Chromium build.")

include(cmake/setup.cmake)

add_subdirectory(library)
add_executable(simple simple_main.cpp)
target_link_libraries(simple
    PUBLIC
        ipfs_client
)
if(IS_DIRECTORY "${CHROMIUM_SOURCE_TREE}")
    add_subdirectory(component)

    set(target_dir "${CHROMIUM_SOURCE_TREE}/components/ipfs/${basename}")
    file(
        COPY
            component/
        DESTINATION
            "${target_dir}"
    )
    file(
        GLOB
            sources
        CONFIGURE_DEPENDS
            component/*.cc
            component/*.h
            library/src/*.cc
    )
    install(
        FILES
            ${sources}
        DESTINATION
            ${target_dir}
    )
    install(
        DIRECTORY
            library/include/ipfs_client/
        DESTINATION
            "${target_dir}/ipfs_client/"
    )
    foreach(source ${sources})
        get_filename_component(basename "${source}" NAME )
        set(gn_sources "${gn_sources}  \"${basename}\",\n  ")
    endforeach()
    configure_file(
        BUILD.gn.in
        ${target_dir}/BUILD.gn
        @ONLY
    )

else()
    message(WARNING "CHROMIUM_SOURCE_TREE not pointing to source tree.")
endif()
