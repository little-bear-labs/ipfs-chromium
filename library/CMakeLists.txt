cmake_minimum_required(VERSION 3.22)
project(ipfs_client
    DESCRIPTION "Code related to using IPFS http gateways to resolve ipfs:// and ipns:// URLs"
    LANGUAGES CXX
)

file(GLOB_RECURSE
    sources
    CONFIGURE_DEPENDS
    src/*.cc
)
list(FILTER
    sources
    EXCLUDE
    REGEX .*test.cc
)
file(GLOB_RECURSE
    headers
    include/*.h
)
file(GLOB
    protos
    components/ipfs/*.proto
)
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(
        pb_src
        pb_hdr
        ${protos}
)
set_source_files_properties(
        ${pb_src}
        ${pb_hdr}
        PROPERTIES GENERATED ON
)

add_library(ipfs_client
  STATIC
      ${sources}
      ${headers} #Mostly for IDEs' sake - this used to be considered bad practice but... generator expressions and such
      ${pb_src}
      ${pb_hdr}
  )
add_library(ipfs_client_covered
  EXCLUDE_FROM_ALL
      ${sources}
      ${headers}
      ${pb_src}
      ${pb_hdr}
  )
foreach(libname ipfs_client ipfs_client_covered)
    target_compile_features(${libname}
        PUBLIC
            cxx_std_${CXX_VERSION}
    )
    target_compile_options(${libname}
        PRIVATE
            ${WARNING_FLAGS}
    )
    target_include_directories(${libname}
        BEFORE
        PUBLIC
            include/
    )
    target_include_directories(${libname}
      PRIVATE
        src/
    )
    with_vocab(${libname})
endforeach()

file(
  GLOB pub_incs
  include/*
  LIST_DIRECTORIES true
)
install(
  DIRECTORY ${pub_incs}
  TYPE INCLUDE
)
install(TARGETS ipfs_client)

set(CPACK_GENERATOR TGZ)
find_program(DPKG_BUILDPACKAGE dpkg-buildpackage)
if(DPKG_BUILDPACKAGE)
    list(APPEND CPACK_GENERATOR DEB)
endif()
find_program(RPMBUILD rpmbuild)
if(RPMBUILD)
    list(APPEND CPACK_GENERATOR RPM)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND DRAGNDROP)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Release)
    set(CPACK_PACKAGE_NAME "ipfs-client-devel")
else()
    set(CPACK_PACKAGE_NAME "ipfs-client-devel-${CMAKE_BUILD_TYPE}")
endif()
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "LBL")
set(CPACK_PACKAGE_CONTACT "john@littlebearlabs.io")
include(CPack)

add_custom_target(upload_library
  DEPENDS package
  COMMENT "Sending library to gcloud"
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/cmake/upload.py" "${CMAKE_PROJECT_VERSION}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  include(CodeCoverage)
  append_coverage_compiler_flags_to_target(ipfs_client_covered)
endif()

file(GLOB_RECURSE
    unit_test_sources
    CONFIGURE_DEPENDS
    src/*_unittest.cc
)
find_package(GTest)
if(GTest_FOUND)
    enable_testing()
    add_executable(unit_test_runner
      EXCLUDE_FROM_ALL
      ${unit_test_sources}
    )
    target_compile_features(unit_test_runner
      PUBLIC
      cxx_std_${CXX_VERSION}
      )
    target_include_directories(unit_test_runner
      PRIVATE
      include/
      src/
      )
    target_link_libraries(unit_test_runner
      PUBLIC
      ipfs_client_covered
      GTest::gtest
      GTest::gtest_main
      )
    with_vocab(unit_test_runner)
    add_custom_target(run_tests
      DEPENDS unit_test_runner
      COMMAND $<TARGET_FILE:unit_test_runner>
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
    add_custom_command(
      OUTPUT ut.cert
      DEPENDS unit_test_runner
      COMMAND $<TARGET_FILE:unit_test_runner>
      COMMAND "${CMAKE_COMMAND}" -E touch ut.cert
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
    if(TEST_BY_DEFAULT)
        add_custom_target(ut
          ALL
          DEPENDS ut.cert
          COMMAND "${CMAKE_COMMAND}" -E echo "Tests up-to-date"
        )
    else()
        add_custom_target(ut
          DEPENDS ut.cert
          COMMAND "${CMAKE_COMMAND}" -E echo "Tests up-to-date"
        )
    endif()
    if(LCOV_PATH)
        setup_target_for_coverage_lcov(
          NAME cov
          DEPENDENCIES run_tests
          EXECUTABLE $<TARGET_FILE:unit_test_runner>
        )
    else()
        message(WARNING "Cannot provide coverage report without lcov")
    endif()
else()
    message(WARNING "Not running tests - no gtest available.")
endif()#GTest_FOUND
