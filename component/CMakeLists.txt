file(GLOB
        ide_srcs
    CONFIGURE_DEPENDS
        *.cc *.h
)
add_library(dont_use
    ${ide_srcs}
)
target_compile_features(dont_use
    PRIVATE
        cxx_std_20
)
target_compile_options(dont_use
    PUBLIC
        -nostdinc++
        -fno-rtti
)
target_include_directories(dont_use
    SYSTEM
    PUBLIC
        ../library/include
        "${CHROMIUM_SOURCE_TREE}"
        "${CHROMIUM_SOURCE_TREE}/buildtools/third_party/libc++/trunk/include/"
        "${CHROMIUM_SOURCE_TREE}/buildtools/third_party/libc++/"
        "${CHROMIUM_SOURCE_TREE}/third_party/abseil-cpp/"
        "${CHROMIUM_SOURCE_TREE}/third_party/boringssl/src/include/"
        "${CHROMIUM_SOURCE_TREE}/out/${CHROMIUM_PROFILE}/gen"
)
#Can't accept its verison of abseil being in the path
#target_link_libraries(dont_use
#    PUBLIC
#        ipfs_client
#)
