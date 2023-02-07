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
        ${Protobuf_LIBRARIES}
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

protobuf_generate_cpp(
    pb_src
    pb_hdr
    src/pb-dag.proto
)
add_custom_command(
    TARGET ipfs_client
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${pb_hdr}" "${CMAKE_CURRENT_LIST_DIR}/include/ipfs_client/gen"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${pb_src}" "${CMAKE_CURRENT_LIST_DIR}/src/gen"
)
