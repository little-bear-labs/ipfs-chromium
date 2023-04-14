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
    COMMAND "${GIT_EXECUTABLE}" -C "${CHROMIUM_SOURCE_TREE}" diff --patch > "${CMAKE_CURRENT_LIST_DIR}/rest_of_chromium.patch"
    COMMENT "Creating patch file (will overwrite)"
)

#Chromium ninja.py targets:
add_custom_target(in_tree_gen
    COMMENT "Building targets necessary to have generated headers used by //components/ipfs"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" components/ipfs:ipfs_generated_code
    SOURCES ${CMAKE_CURRENT_BINARY_DIR}/BUILD.gn
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
    add_custom_target(package
        COMMENT "Packaging"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" "chrome/installer"
        DEPENDS chrome_browser
      )
endif()

#Symlink for convenience
add_custom_command(
  TARGET chrome_browser
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E create_symlink "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/chrome" "${CMAKE_BINARY_DIR}/chrome_browser"
  COMMENT "Symlinking... your browser can be found here: ${CMAKE_BINARY_DIR}/chrome_browser or ${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/chrome"
)
