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

file(
    READ "${CHROMIUM_SOURCE_TREE}/chrome/browser/BUILD.gn"
    browser_build
)
string(
    FIND
        "${browser_build}"
        ipfs
        ipfs_dep
)
if(ipfs_dep GREATER -1)
    message(STATUS "Looks like you might have already patched Chromium.")
elseif(GIT_EXECUTABLE)
    message(WARNING "ipfs dependency not found in chrome/browser/BUILD.gn. Assuming your Chromium source is not patched - will attempt to apply a patch right now.")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" apply "${CMAKE_CURRENT_LIST_DIR}/rest_of_chromium.patch"
        WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}"
	COMMAND_ERROR_IS_FATAL ANY
    )
elseif()
    message(FATAL_ERROR "It would appear your Chromium source tree is unpatched, and I don't have access to git.")
endif()

set(target_dir "${CHROMIUM_SOURCE_TREE}/components/ipfs/${basename}")
file(GLOB lib_srcs ../library/src/*.cc)
file(GLOB_RECURSE
    lib_hdrs_rel
    RELATIVE "${CMAKE_SOURCE_DIR}/library/include"
    "${CMAKE_SOURCE_DIR}/library/include/*.h"
)
file(GLOB_RECURSE
    lib_hdrs_abs
    "${CMAKE_SOURCE_DIR}/library/include/*.h"
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
    SOURCES ${srcs} ${hdrs} ${lib_srcs} ${lib_hdrs_abs}
)

foreach(hdr ${lib_hdrs_rel})
    get_filename_component(dir "${hdr}" DIRECTORY)
    file(MAKE_DIRECTORY
        "${target_dir}/${dir}"
    )
    add_custom_command(TARGET setup_component
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/library/include/${hdr}" "${target_dir}/${dir}"
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
