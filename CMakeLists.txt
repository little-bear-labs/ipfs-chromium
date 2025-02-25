cmake_minimum_required(VERSION 3.22)
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(python)
execute_process(
    COMMAND "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/bld/ic_version.py"
    OUTPUT_VARIABLE deduced_version
  )
project(ipfs_client
    DESCRIPTION "Code related to using IPFS http gateways to resolve ipfs:// and ipns:// URLs"
    LANGUAGES CXX
    VERSION ${deduced_version}
)

set(TESTING ON CACHE BOOL "Do we want to run tests?")
set(CXX_VERSION 20 CACHE STRING "Should be no less than your conan profile.")

file(READ bld/protobuf_version.txt pb_ver)
string(STRIP "${pb_ver}" pb_ver)

include(dependencies)

dependency(protobuf VERSION ${pb_ver})
dependency(absl)
dependency(Boost)
dependency(c-ares)
dependency(nlohmann_json)
dependency(OpenSSL)
if(TESTING)
  dependency(GTest)
endif()

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
    include/*.hpp
)
file(GLOB_RECURSE
  protos
  proto/*.proto
)

foreach(proto ${protos})
    get_filename_component(base "${proto}" NAME_WE)
    set(d "${CMAKE_CURRENT_BINARY_DIR}/gen")
    set(p "${d}/ipfs_client/${base}.pb")
    add_custom_command(
        OUTPUT "${p}.h" "${p}.cc"
        MAIN_DEPENDENCY "${proto}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${d}"
        COMMAND bash -ex "${CMAKE_CURRENT_SOURCE_DIR}/bld/protoc.sh" "${CMAKE_CURRENT_SOURCE_DIR}/proto" "${d}" "${proto}"
    )
    list(APPEND pb_gen "${p}.h" )
    list(APPEND pb_gen "${p}.cc")
    set_source_files_properties("${p}.h" PROPERTIES GENERATED ON)
    set_source_files_properties("${p}.cc" PROPERTIES GENERATED ON)
    set(gn_protos "${gn_protos}      \"proto/ipfs_client/${base}.proto\", \n ")
endforeach()
set_source_files_properties(${pb_gen} PROPERTIES GENERATED ON)

add_library(ic_proto
    ${pb_gen}
)
target_compile_features(ic_proto
    PUBLIC
        cxx_std_${CXX_VERSION}
)
target_include_directories(ic_proto
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}/gen"
        "${protobuf_INCLUDE_DIR}"
)
target_link_libraries(ic_proto
  PUBLIC
    protobuf::protobuf
)

add_library(ipfs_client
  STATIC
      ${sources}
      ${headers} #Mostly for IDEs' sake - this used to be considered bad practice but... generator expressions and such
  )
if(USE_CLANG_TIDY AND CLANG_TIDY_EXE)
  set_target_properties(ipfs_client
    PROPERTIES
      CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy;--fix"
    )
endif()
add_library(ipfs_client_covered
  EXCLUDE_FROM_ALL
      ${sources}
      ${headers}
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
    target_compile_definitions(${libname}
      PRIVATE
        ABSL_USES_STD_STRING_VIEW=1
    )
    target_include_directories(${libname}
        PUBLIC
            include/
    )
    target_include_directories(${libname}
        SYSTEM
        PUBLIC
            "${CMAKE_CURRENT_BINARY_DIR}/gen"
            "${CMAKE_CURRENT_BINARY_DIR}"
    )
    target_include_directories(${libname}
      PRIVATE
        src/
    )
    target_link_libraries(${libname}
      PUBLIC
        ic_proto
    )
    target_link_libraries(${libname}
      PUBLIC
        absl::strings
        Boost::headers
        c-ares::cares
        nlohmann_json::nlohmann_json
    )
endforeach()

foreach(source ${sources})
  cmake_path(RELATIVE_PATH
      source
      OUTPUT_VARIABLE relsrc
      BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
  string(FIND "${relsrc}" "opinion" i)
  string(FIND "${relsrc}" "logger" j)
  if(i EQUAL j)
    set(gn_sources "${gn_sources}      \"${relsrc}\", \n ")
  endif()
endforeach()
configure_file(
    bld/BUILD.gn.in
    "${CMAKE_CURRENT_SOURCE_DIR}/BUILD.gn"
    @ONLY
  )

file(
  GLOB pub_incs
  include/*
  LIST_DIRECTORIES true
)
install(
  DIRECTORY ${pub_incs}
  TYPE INCLUDE
)
install(TARGETS ic_proto)
install(TARGETS ipfs_client)

set(CPACK_GENERATOR TGZ;ZIP)
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
set(CPACK_ARCHIVE_FILE_NAME "ipfs-client")
include(CPack)

file(GLOB_RECURSE
    unit_test_sources
    CONFIGURE_DEPENDS
    src/*_unittest.cc
)
if(TESTING)
    get_target_property(links ipfs_client_covered INTERFACE_LINK_LIBRARIES)
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
      test_data/include
      )
    target_link_libraries(unit_test_runner
      PUBLIC
      ipfs_client_covered
      OpenSSL::SSL
      GTest::gtest
      GTest::gtest_main
      )
    find_program(VALGRIND "valgrind")
    add_custom_target(run_tests
      DEPENDS unit_test_runner
      COMMAND $<TARGET_FILE:unit_test_runner>
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
    if(VALGRIND)
        add_custom_command(
          OUTPUT ut.cert
          DEPENDS unit_test_runner
          COMMAND "${VALGRIND}" --tool=memcheck --leak-check=no --exit-on-first-error=yes --error-exitcode=99 "--suppressions=${CMAKE_SOURCE_DIR}/bld/valgrind.suppressions.txt"  $<TARGET_FILE:unit_test_runner>
          COMMAND "${CMAKE_COMMAND}" -E touch ut.cert
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
        )
    else()
        add_custom_command(
          OUTPUT ut.cert
          DEPENDS unit_test_runner
          COMMAND $<TARGET_FILE:unit_test_runner>
          COMMAND "${CMAKE_COMMAND}" -E touch ut.cert
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
        )
    endif()
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
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        include(CodeCoverage OPTIONAL RESULT_VARIABLE INCLUDED_COVERAGE)
        if(INCLUDED_COVERAGE)
            append_coverage_compiler_flags_to_target(ipfs_client_covered)
            append_coverage_compiler_flags_to_target(unit_test_runner)
            if(CMAKE_BUILD_TYPE STREQUAL Debug)
                target_compile_options(ipfs_client_covered
                        PRIVATE
                        ${coverage_debug_flags}
                )
            endif()
            if(CMAKE_BUILD_TYPE STREQUAL Debug)
                target_compile_options(unit_test_runner
                        PRIVATE
                        ${coverage_debug_flags}
                )
            endif()
        endif()
        if(LCOV_PATH)
            setup_target_for_coverage_lcov(
                NAME cov
                DEPENDENCIES run_tests
                EXECUTABLE $<TARGET_FILE:unit_test_runner>
                EXCLUDE ${unit_test_sources}
                LCOV_ARGS --ignore-errors unused
            )
        else()
            message(WARNING "Cannot provide coverage report without lcov")
        endif()
    endif()
endif()

include(cmake/doxy.cmake)
include(cmake/marp.cmake)
if(doc_targets)
    add_custom_target(doc
      DEPENDS ${doc_targets}
      COMMENT "Generated documentation: ${doc_targets}"
    )
endif()

