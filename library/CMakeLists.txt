cmake_minimum_required(VERSION 3.24)
project(ipfs_client
    DESCRIPTION "Code related to using IPFS http gateways to resolve ipfs:// and ipns:// URLs"
    VERSION 0.0.0.0
    LANGUAGES CXX
)

file(GLOB
    sources
    CONFIGURE_DEPENDS
    src/*.cc
)
add_library(ipfs_client
    ${sources}
)
target_compile_features(ipfs_client
    PUBLIC
        cxx_std_20
)
target_include_directories(ipfs_client
    PUBLIC
        include/
)

find_package(absl)
if(absl_FOUND)
    target_link_libraries(ipfs_client
        PUBLIC
            absl::flat_hash_map
            absl::flat_hash_set
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
