file(GLOB
    srcs
    CONFIGURE_DEPENDS
    *.cc
)
file(GLOB
    hdrs
    CONFIGURE_DEPENDS
    *.h
)

#Mostly for the sake of IDEs, though it also serves a purpose here.
add_library(local_shim_build
    ${srcs}
    ${hdrs} #Some IDEs like you to indicate header association this way. Otherwise they're "external"
)
target_compile_features(local_shim_build
    PRIVATE
        cxx_std_20
)
target_compile_options(local_shim_build
    PUBLIC
        ${CHROMIUM_COMPILER_FLAGS}
        ${WARNING_FLAGS}
)
target_include_directories(local_shim_build
    SYSTEM
    PUBLIC
        "${CHROMIUM_SOURCE_TREE}"
        "${CHROMIUM_SOURCE_TREE}/buildtools/third_party/libc++/trunk/include/"
        "${CHROMIUM_SOURCE_TREE}/buildtools/third_party/libc++/"
        "${CHROMIUM_SOURCE_TREE}/third_party/abseil-cpp/"
        "${CHROMIUM_SOURCE_TREE}/third_party/boringssl/src/include/"
        "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/gen"
)
target_link_libraries(local_shim_build
    PUBLIC
        ipfs_client
)

set(target_dir "${CHROMIUM_SOURCE_TREE}/components/ipfs/${basename}")
file(GLOB lib_srcs ../library/src/*.cc)
file(GLOB_RECURSE
    lib_hdrs
    RELATIVE ../library/include/
    ../library/include/*.h
)

foreach(source ${srcs} ${lib_srcs})
    get_filename_component(basename "${source}" NAME )
    set(gn_sources "${gn_sources}  \"${basename}\",\n  ")
endforeach()
configure_file(
    BUILD.gn.in
    ${CMAKE_CURRENT_BINARY_DIR}/BUILD.gn
    @ONLY
)
add_custom_target(setup_component
    ALL
    DEPENDS local_shim_build
    COMMENT "Moving code over to ${target_dir} and setting up the build there."
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${srcs} ${hdrs} ${lib_srcs} ${CMAKE_CURRENT_BINARY_DIR}/BUILD.gn "${target_dir}"
)
foreach(hdr ${lib_hdrs})
    get_filename_component(dir "${hdr}" DIRECTORY)
    file(MAKE_DIRECTORY
        "${target_dir}/${dir}"
    )
    add_custom_command(TARGET setup_component
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}../library/include/${hdr}" "${target_dir}/${dir}"
    )
endforeach()
if(GIT_EXECUTABLE)
    add_custom_command(
        TARGET setup_component
        POST_BUILD
        WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}"
        COMMAND "${GIT_EXECUTABLE}" diff --patch > "${CMAKE_CURRENT_LIST_DIR}/rest_of_chromium.patch"
    )
else()
    message(WARNING "git not in path - will not be creating the patch file to save your changes.")
endif()
if(Python3_EXECUTABLE)
    if(DEPOT_TOOLS_NINJA_PY)
        add_custom_target(in_tree_build
            ALL
            DEPENDS setup_component
            WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}"
            COMMENT "Building component in Chromium source tree ${target_dir}, output in out/${CHROMIUM_PROFILE}"
            COMMAND "${Python3_EXECUTABLE}" "${DEPOT_TOOLS_NINJA_PY}" -j 9 components/ipfs
        )
    else()
        message(WARNING "No depot_tools")
    endif()
else()
    message(WARNING "No python3")
endif()
