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
        *.proto
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
    ${sources}
    ${headers} #Mostly for IDEs' sake - this used to be considered bad practice but... generator expressions and such
    ${pb_src}
    ${pb_hdr}
)
target_compile_features(ipfs_client
    PUBLIC
        cxx_std_${CXX_VERSION}
)
target_compile_options(ipfs_client
    PRIVATE
        ${WARNING_FLAGS}
)
target_include_directories(ipfs_client
    BEFORE
    PUBLIC
        include/
)
target_include_directories(ipfs_client
  PRIVATE
    src/
)
with_vocab(ipfs_client)
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

set(CPACK_GENERATOR TGZ)#;ZIP;DEB;RPM)
set(CPACK_PACKAGE_NAME "ipfs-client-dev-${CMAKE_BUILD_TYPE}")
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

file(GLOB_RECURSE
    unit_test_sources
    CONFIGURE_DEPENDS
    src/*_unittest.cc
)
find_package(GTest)

if(GTest_FOUND)
    include(CodeCoverage)
    enable_testing()
    add_custom_target(run_tests)
    foreach(unit_test_source ${unit_test_sources})
        get_filename_component(name ${unit_test_source} NAME_WE)
        string(REPLACE _unittest "" source_under_test ${unit_test_source})
        if(EXISTS "${source_under_test}")
            list(APPEND unit_test_source "${source_under_test}")
        endif()
        add_executable(${name}_runner
            ${unit_test_source}
        )
        target_compile_features(${name}_runner
            PUBLIC
                cxx_std_${CXX_VERSION}
        )
        target_include_directories(${name}_runner
          PRIVATE
            include/
            src/
        )
        target_link_libraries(${name}_runner
            PUBLIC
                ipfs_client
                GTest::gtest
                GTest::gtest_main
        )
        with_vocab(${name}_runner)
        #message(WARNING "Calling append_coverage_compiler_flags_to_target(${name}_runner)")
        append_coverage_compiler_flags_to_target(${name}_runner)
        add_test(
            NAME ${name}_runner
            COMMAND $<TARGET_FILE:${name}_runner>
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        )
        add_custom_command(
            OUTPUT ${name}_cert
            DEPENDS ${name}_runner
            COMMAND $<TARGET_FILE:${name}_runner>
            COMMAND "${CMAKE_COMMAND}" -E touch ${name}_cert
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        )
        list(APPEND test_certificates ${name}_cert)
        add_custom_target(run_${name}
          DEPENDS ${name}_runner
          COMMAND $<TARGET_FILE:${name}_runner>
          WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        )
        add_dependencies(run_tests run_${name})
    endforeach()
    add_custom_target(check_tests
#        ALL
        SOURCES ${test_certificates}
        COMMAND echo "Tests are up-to-date."
    )
else()
    message(WARNING "Not running tests - no gtest available.")
endif()#GTest_FOUND
setup_target_for_coverage_lcov(
  NAME cov
  DEPENDENCIES run_tests
)
