#First gather various variables that will be useful in the configure_file...
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

configure_file(
    BUILD.gn.in
    ${CMAKE_CURRENT_BINARY_DIR}/BUILD.gn
    @ONLY
)

add_custom_target(patch_file
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/patch.py" "${CHROMIUM_SOURCE_TREE}" "${GIT_EXECUTABLE}" "${CMAKE_BUILD_TYPE}"
  COMMENT "Creating patch file (will overwrite if already present)."
  )


add_custom_target(setup_component
    COMMENT "Preparing the Chromium repository & build directory, but not building any targets."
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" 
    SOURCES ${CMAKE_CURRENT_BINARY_DIR}/BUILD.gn
  )
#Chromium ninja.py targets:
add_custom_target(in_tree_gen
    COMMENT "Building targets necessary to have generated headers used by //components/ipfs"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" components/ipfs:ipfs_generated_code
    DEPENDS setup_component
  )
add_custom_target(in_tree_build
    ALL
    COMMENT "Building component in Chromium source tree ${target_dir}, output in out/${CHROMIUM_PROFILE}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" components/ipfs:ipfs
    DEPENDS in_tree_gen
  )
add_custom_target(chrome_browser
    COMMENT "Building 'chrome' in Chromium source tree, output in out/${CHROMIUM_PROFILE}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" chrome
    DEPENDS in_tree_build
  )

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_target(package_browser
        COMMENT "Packaging"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" "chrome/installer"
        DEPENDS chrome_browser
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
  TARGET chrome_browser
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E create_symlink "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/chrome" "${CMAKE_BINARY_DIR}/chrome_browser"
  COMMENT "Symlinking... your browser can be found here: ${CMAKE_BINARY_DIR}/chrome_browser or ${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/chrome"
)

add_custom_target(inc_link
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/inc_link.py" "${CMAKE_BINARY_DIR}"
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
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
  PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}/inc_link"
  )
add_dependencies(out_of_tree inc_link)
