cmake_minimum_required(VERSION 3.24)
project(ipfs_client
    DESCRIPTION "Code related to using IPFS http gateways to resolve ipfs:// and ipns:// URLs"
    VERSION 0.0.0.0
    LANGUAGES CXX
)

file(GLOB_RECURSE
    sources
    CONFIGURE_DEPENDS
    src/*.cc
    src/*.cpp
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
add_library(ipfs_client
    ${sources}
    ${headers} #Mostly for IDEs' sake - this used to be considered bad practice but... generator expressions and such
)
target_compile_features(ipfs_client
    PUBLIC
        cxx_std_20
)
target_compile_options(ipfs_client
    PRIVATE
        ${WARNING_FLAGS}
)
target_include_directories(ipfs_client
    PUBLIC
        include/
)
target_include_directories(ipfs_client
    PRIVATE
    SYSTEM
        include/ipfs_client/gen/
)
target_link_libraries(ipfs_client
    PUBLIC
        protobuf::libprotobuf
)

find_package(absl)
if(absl_FOUND)
    target_link_libraries(ipfs_client
        PRIVATE # It is conceptually public, that is to say clients need abseil, but ../component can't use *this* abseil
            absl::flat_hash_map
            absl::flat_hash_set
            absl::statusor
    )
endif()

find_package(Boost
    COMPONENTS
        container
)
if(Boost_FOUND)
    target_link_libraries(ipfs_client
        PUBLIC
            Boost::boost
    )
endif()

file(GLOB
    protos
    *.proto
)
protobuf_generate_cpp(
    pb_src
    pb_hdr
    ${protos}
)
add_custom_target(update_pb_source_in_repo
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${pb_hdr}" "${CMAKE_CURRENT_LIST_DIR}/include/ipfs_client/gen"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${pb_src}" "${CMAKE_CURRENT_LIST_DIR}/src/gen"
    COMMAND_EXPAND_LISTS
    SOURCES
        "${pb_src}"
        "${pb_hdr}"
)
add_dependencies(ipfs_client update_pb_source_in_repo)

file(GLOB_RECURSE
    unit_test_sources
    CONFIGURE_DEPENDS
    src/*_unittest.cc
)
find_package(GTest)
if(GTest_FOUND)
if(absl_FOUND)
    enable_testing()
    foreach(unit_test_source ${unit_test_sources})
        get_filename_component(name ${unit_test_source} NAME_WE)
        add_executable(${name}_runner
            ${unit_test_source}
        )
        target_link_libraries(${name}_runner
            PUBLIC
                ipfs_client
                GTest::gtest
                GTest::gtest_main
                absl::statusor
        )
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
    endforeach()
    add_custom_target(check_tests
        ALL
        SOURCES ${test_certificates}
        COMMAND echo "Tests are up-to-date."
    )
endif()#absl_FOUND
endif()#GTest_FOUND
