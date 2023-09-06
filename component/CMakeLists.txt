
if(NOT CHROMIUM_PROFILE)
    message(FATAL_ERROR "Your Chromium profile should name a realistic subdir of chromium/src/out, perhaps Debug, Release, or Default")
endif()
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

add_custom_target(patch_file
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/patch.py" "${CHROMIUM_SOURCE_TREE}" "${GIT_EXECUTABLE}" "${CMAKE_BUILD_TYPE}"
  COMMENT "Creating patch file (will overwrite if already present)."
  )

add_custom_target(setup_in_tree
    COMMENT "Preparing the Chromium repository & build directory, but not building any targets."
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" base
  )
#Chromium ninja.py targets:
add_custom_target(in_tree_gen
    COMMENT "Building targets necessary to have generated headers...."
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" third_party/ipfs_client:protos
    DEPENDS setup_in_tree
  )
add_custom_target(in_tree_lib
    COMMENT "Building ipfs-client in-tree"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" third_party/ipfs_client:ipfs_client
    DEPENDS in_tree_gen
  )
add_custom_target(in_tree_build
    ALL
    COMMENT "Building component in Chromium source tree ${target_dir}, output in out/${CHROMIUM_PROFILE}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" components/ipfs:ipfs
    DEPENDS in_tree_lib
  )
add_custom_target(chromium_ordering_issue
    COMMENT "For some versions of Chromium the internal build dependency order isn't perfect."
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" ui/webui/resources/cr_components/color_change_listener:mojom
    DEPENDS in_tree_build
  )
add_custom_target(build_browser
    COMMENT "Building 'chrome' in Chromium source tree, output in out/${CHROMIUM_PROFILE}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" chrome
    DEPENDS chromium_ordering_issue
  )

if(CHROMIUM_PROFILE STREQUAL win)
    set(CHROMIUM_PACKAGE_TARGET mini_installer)
else()
    set(CHROMIUM_PACKAGE_TARGET chrome/installer)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_target(package_browser
        COMMENT "Packaging"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" ${CHROMIUM_PACKAGE_TARGET}
        DEPENDS build_browser
      )
    add_custom_target(upload_browser
      DEPENDS package_browser
      COMMENT "Sending browser installer to gcloud"
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/upload.py" "${CMAKE_PROJECT_VERSION}"
      WORKING_DIRECTORY "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}"
    )
endif()

#Symlink for convenience
add_custom_command(
  TARGET build_browser
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E create_symlink "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/chrome" "${CMAKE_BINARY_DIR}/browser"
  COMMENT "Symlinking... your browser can be found here: ${CMAKE_BINARY_DIR}/browser or ${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/chrome"
)

add_custom_target(inc_link
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/inc_link.py" "${CMAKE_BINARY_DIR}"
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  DEPENDS in_tree_gen
  )

add_library(out_of_tree
  EXCLUDE_FROM_ALL
  ${srcs}
  )
target_compile_features(out_of_tree
  PUBLIC
    #cxx_std_${CXX_VERSION}
    cxx_std_20
  )
target_compile_options(out_of_tree
  PUBLIC
    ${WARNING_FLAGS}
  )
target_link_libraries(out_of_tree
  PUBLIC
    ipfs_client
  )
target_include_directories(out_of_tree
  SYSTEM
  BEFORE
  PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/inc_link"
  )
target_include_directories(out_of_tree
  PUBLIC
  ../library/src
  )
add_dependencies(out_of_tree inc_link)

