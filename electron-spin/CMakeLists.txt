cmake_minimum_required(VERSION 3.22)

project(electron_spin_standalone_testing
    LANGUAGES CXX
)

file(GLOB
    srcs
    CONFIGURE_DEPENDS
    *.cc
)

add_library(electron_spin_standalone
    EXCLUDE_FROM_ALL
        ${srcs}
)
target_link_libraries(electron_spin_standalone
    PUBLIC
        out_of_tree
)