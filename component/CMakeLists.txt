file(GLOB
    srcs
    CONFIGURE_DEPENDS
    *.cc
)
list(FILTER
    srcs
    EXCLUDE
    REGEX
    ".*test.cc$"
)
file(GLOB
    hdrs
    CONFIGURE_DEPENDS
    *.h
)
file(GLOB
    test_srcs
    CONFIGURE_DEPENDS
    *test.cc
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

set(target_dir "${CHROMIUM_SOURCE_TREE}/components/ipfs")
file(GLOB_RECURSE
    lib_srcs
        ../library/src/*.cc
        ../library/src/*.h #Private headers
)
list(FILTER
    lib_srcs
    EXCLUDE
    REGEX .*test.cc
)
file(GLOB_RECURSE
    lib_hdrs_rel
    RELATIVE "${CMAKE_SOURCE_DIR}/library/include"
    "${CMAKE_SOURCE_DIR}/library/include/*.h"
    "${CMAKE_SOURCE_DIR}/library/src/*.h"
    "${CMAKE_SOURCE_DIR}/library/include/*.hpp"
)
file(GLOB_RECURSE
    lib_hdrs_abs
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/library/include/*.h"
    "${CMAKE_SOURCE_DIR}/library/src/*.h"
    "${CMAKE_SOURCE_DIR}/library/include/*.hpp"
)
file(GLOB
  protos
  ${CMAKE_SOURCE_DIR}/library/*.proto
  )

foreach(source ${srcs} ${lib_srcs})
    get_filename_component(basename "${source}" NAME )
    set(gn_sources "${gn_sources}  \"${basename}\",\n  ")
endforeach()
foreach(source ${srcs} ${test_srcs})
    get_filename_component(basename "${source}" NAME )
    set(gn_test_sources "${gn_test_sources}  \"${basename}\",\n  ")
endforeach()
foreach(proto ${protos} )
    get_filename_component(basename "${proto}" NAME )
    set(gn_protos "${gn_protos}  \"${basename}\",\n  ")
endforeach()
configure_file(
    BUILD.gn.in
    ${CMAKE_CURRENT_BINARY_DIR}/BUILD.gn
    @ONLY
)
add_custom_target(setup_component
    ALL
    COMMENT "Moving code over to ${target_dir} and setting up the build there."
    COMMAND ${CMAKE_COMMAND} -E make_directory "${target_dir}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${srcs} ${hdrs} ${lib_srcs} ${test_srcs} ${protos} ${CMAKE_CURRENT_BINARY_DIR}/BUILD.gn "${target_dir}"
    SOURCES ${srcs} ${hdrs} ${lib_srcs} ${lib_hdrs_abs}
    BYPRODUCTS "${target_dir}/BUILD.gn"
)

foreach(hdr ${lib_hdrs_rel})
    get_filename_component(dir "${hdr}" DIRECTORY)
    if (dir MATCHES .*/gen)
        message(VERBOSE "Not including ${dir} in the include directories to sync with the Chromium tree. It should generate those files on its own.")
    else()
        file(MAKE_DIRECTORY
            "${target_dir}/${dir}"
        )
        add_custom_command(TARGET setup_component
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/library/include/${hdr}" "${target_dir}/${dir}"
        )
    endif ()
endforeach()
if(GIT_EXECUTABLE)
#    add_custom_command(
#        TARGET setup_component
#        POST_BUILD
#        WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}"
    add_custom_target(patch_file
        COMMAND "${GIT_EXECUTABLE}" -C "${CHROMIUM_SOURCE_TREE}" diff --patch > "${CMAKE_CURRENT_LIST_DIR}/rest_of_chromium.patch"
        DEPENDS setup_component
    )
else()
    message(WARNING "git not in path - will not be creating the patch file to save your changes.")
endif()
add_custom_command(
  COMMENT "Generating build files including //components/ipfs"
  OUTPUT "${chromium_build_dir}/obj/components/ipfs/ipfs.ninja"
  COMMAND "${Python3_EXECUTABLE}" "${DEPOT_TOOLS_GN_PY}" gen "--args= ${gnargs} " "out/${CHROMIUM_PROFILE}"
  MAIN_DEPENDENCY "${target_dir}/BUILD.gn"
  WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}"
  VERBATIM
)
add_custom_command(
  COMMENT "Building targets necessary to have generated headers used by //components/ipfs"
  OUTPUT "${chromium_build_dir}/gen/components/ipfs/pb_dag.pb.h"
  COMMAND "${Python3_EXECUTABLE}" "${DEPOT_TOOLS_NINJA_PY}" -j ${parallel_jobs} components/ipfs:ipfs_generated_code
  MAIN_DEPENDENCY "${chromium_build_dir}/obj/components/ipfs/ipfs.ninja"
  WORKING_DIRECTORY "${chromium_build_dir}"
)
add_custom_target(generate_in_tree_headers
  ALL
  COMMENT "Checking that the headers are generated via gn-generated Ninja build."
  SOURCES "${chromium_build_dir}/gen/components/ipfs/pb_dag.pb.h"
)

#Mostly for the sake of IDEs
#set(stubbedchromiumincdir "/" CACHE PATH "Used only for local_shim_build")
#add_library(local_shim_build
#  ${srcs}
#  ${hdrs} #Some IDEs like you to indicate header association this way. Otherwise they're "external"
#)
#target_compile_features(local_shim_build
#  PRIVATE
#    cxx_std_17 # Always target C++17 when targeting Chromium
#)
#target_compile_options(local_shim_build
#  PUBLIC
#    ${WARNING_FLAGS}
#)
#target_link_libraries(local_shim_build
#  PUBLIC
#    ipfs_client
#)
#target_include_directories(local_shim_build
#  SYSTEM
#  PUBLIC
#    .
#    "${CMAKE_BINARY_DIR}"
#    ${stubbedchromiumincdir}
#)

if(DEPOT_TOOLS_NINJA_PY)
    add_custom_target(in_tree_build
        ALL
        DEPENDS setup_component
        WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}"
        COMMENT "Building component in Chromium source tree ${target_dir}, output in out/${CHROMIUM_PROFILE}"
        COMMAND "${Python3_EXECUTABLE}" "${DEPOT_TOOLS_NINJA_PY}" -j ${parallel_jobs} components/ipfs:ipfs
    )
    add_custom_target(chrome_browser
      DEPENDS in_tree_build
      WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}"
      COMMENT "Building 'chrome' in Chromium source tree, output in out/${CHROMIUM_PROFILE}"
      COMMAND "${Python3_EXECUTABLE}" "${DEPOT_TOOLS_NINJA_PY}" -j ${parallel_jobs} chrome
    )
else()
    message(WARNING "No depot_tools")
endif()

file(GLOB
    chromium_libs
    "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/lib*.so"
    "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/obj/t*/*/lib*.a"
)
if(ENABLE_OUTOFTREE_TESTING)#TODO
    enable_testing()
    foreach(test_src ${test_srcs})
        get_filename_component(
            name
            ${test_srcs}
            NAME_WE
        )
        add_executable(${name}_runner
            ${test_src}
        )
        target_compile_features(${name}_runner
            PUBLIC
            cxx_std_${CXX_VERSION}
        )
        target_include_directories(${name}_runner
            PUBLIC
                "${CHROMIUM_SOURCE_TREE}/third_party/googletest/src/googletest/include"
        )
        target_link_libraries(${name}_runner
            PUBLIC
                local_shim_build
                ${chromium_libs}
        )
        add_test(
            NAME ${name}
            COMMAND "$<TARGET_FILE:${name}_runner>"
        )
        add_custom_command(
            OUTPUT ${name}.cert
            COMMAND "$<TARGET_FILE:${name}_runner>"
            COMMAND "${CMAKE_COMMAND}" -E touch ${name}.cert
            DEPENDS ${name}_runner
        )
        list(APPEND certs ${name}.cert)
    endforeach()
    add_custom_target(check_tests
        ALL
        DEPENDS ${certs}
        COMMAND echo "Tests up-to-date."
    )
endif(ENABLE_OUTOFTREE_TESTING)
