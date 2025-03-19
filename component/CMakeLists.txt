include(setup)

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

add_custom_target(record_edits
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/patch.py" "${CHROMIUM_SOURCE_TREE}" "${GIT_EXECUTABLE}" "${CMAKE_BUILD_TYPE}"
  COMMENT "Creating patch files and such (will overwrite if already present)."
  )

# include(FetchContent)
# FetchContent_Declare(fetch_ipfs_client
#   GIT_REPOSITORY "https://github.com/little-bear-labs/ipfs-chromium.git"
#   GIT_TAG "${IPFS_CLIENT_REF}"
# )
# FetchContent_MakeAvailable(fetch_ipfs_client)

add_custom_target(setup_in_tree
    COMMENT "Preparing the Chromium repository & build directory, but not building any targets."
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" base
    # DEPENDS fetch_ipfs_client
  )
add_custom_target(pull_clang_config
    COMMAND "${CMAKE_COMMAND}" -E copy "${CHROMIUM_SOURCE_TREE}/.clang-format" "${CMAKE_SOURCE_DIR}/"
    COMMAND "${CMAKE_COMMAND}" -E copy "${CHROMIUM_SOURCE_TREE}/.clang-tidy"   "${CMAKE_SOURCE_DIR}/"
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
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}"
        ui/webui/resources/cr_components/color_change_listener:mojom
        components/reporting/proto:configuration_file_proto
    DEPENDS in_tree_build
  )
add_custom_target(build_electron
    COMMENT "Building 'chrome' in Chromium source tree, output in out/${CHROMIUM_PROFILE}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" electron
    DEPENDS chromium_ordering_issue
  )
add_custom_target(build_browser
    COMMENT "Building 'chrome' in Chromium source tree, output in out/${CHROMIUM_PROFILE}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" chrome
    DEPENDS chromium_ordering_issue
  )

if(CMAKE_BUILD_TYPE STREQUAL "Release" )
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    add_custom_target(package_browser
        COMMENT "Packaging"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" mini_installer
        DEPENDS build_browser
      )
    add_custom_target(package_electron
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" "electron:electron_dist_zip"
        DEPENDS build_electron
    )
  elseif("${CHROMIUM_PROFILE}" STREQUAL "win")
    add_custom_target(package_browser
        COMMENT "Packaging"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" chrome/installer
        DEPENDS build_browser
      )
    add_custom_target(package_electron
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" "electron:electron_dist_zip"
        DEPENDS build_electron
    )
  else()
    add_custom_target(build_stripped_browser
      COMMENT "Stripping browser, output is out/${CHROMIUM_PROFILE}/chrome.stripped"
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" strip_chrome_binary
      DEPENDS build_browser
    )
    add_custom_target(build_browser_sandbox
      COMMENT "Sandboxing browser, output is out/${CHROMIUM_PROFILE}/chrome_sandbox"
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" chrome_sandbox
      DEPENDS build_stripped_browser
    )
    add_custom_target(build_stripped_browser_sandbox
      COMMENT "Sandboxing stripped browser"
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" chrome_sandbox.stripped
      DEPENDS build_browser_sandbox
    )
    foreach(suffix "" _sandbox)
      add_custom_command(
        TARGET build_browser${suffix}
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy
          "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/chrome${suffix}"
          "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/ipfs_chromium${suffix}"
      )
      add_custom_command(
        TARGET build_stripped_browser${suffix}
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy
          "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/chrome${suffix}.stripped"
          "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/ipfs_chromium${suffix}.stripped"
      )
    endforeach()
    add_custom_target(package_browser
        COMMENT "Packaging"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" chrome/installer
        DEPENDS build_stripped_browser build_stripped_browser_sandbox
      )
    add_custom_target(package_electron
        COMMAND "${Python3_EXECUTABLE}" "${CHROMIUM_SOURCE_TREE}/electron/script/strip-binaries.py" -d "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/chromium.py" "${CMAKE_BINARY_DIR}" "electron:electron_dist_zip"
        DEPENDS build_electron
    )
  endif()
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  #Symlinks for convenience
  add_custom_command(
    TARGET build_browser
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E create_symlink "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/chrome" "${CMAKE_BINARY_DIR}/browser"
    COMMENT "Symlinking... your browser can be found here: ${CMAKE_BINARY_DIR}/browser or ${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/chrome"
  )
endif()
add_custom_command(
  TARGET build_electron
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E create_symlink "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/electron" "${CMAKE_BINARY_DIR}/electron"
)

add_custom_target(inc_link
  ALL
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/inc_link.py" "${CMAKE_BINARY_DIR}"
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  DEPENDS in_tree_gen
  )

add_library(out_of_tree
  EXCLUDE_FROM_ALL
  ${srcs}
  )
target_compile_definitions(out_of_tree
  PRIVATE
    OPENSSL_EXPORT=
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
find_package(
    Protobuf
    "${protobuf_version}"
    QUIET
    REQUIRED
)
find_package(OpenSSL
    QUIET
    COMPONENTS
        Crypto
)
find_package(
  ipfs_client
  REQUIRED
)
target_link_libraries(out_of_tree
  PUBLIC
    protobuf::libprotobuf
    ipfs_client::ipfs_client
    OpenSSL::SSL
  )
target_include_directories(out_of_tree
  SYSTEM
  BEFORE
  PUBLIC
    "../test_data/include"
    "${protobuf_INCLUDE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/inc_link"
  )
target_include_directories(out_of_tree
  PUBLIC
  ../library/src
  )

add_dependencies(out_of_tree inc_link)
