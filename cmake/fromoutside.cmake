set(chromium_external_inc_dirs "${CHROMIUM_SOURCE_TREE}")
list(APPEND chromium_external_inc_dirs "${CHROMIUM_OUT_DIR}/gen")
list(APPEND chromium_external_inc_dirs "${CHROMIUM_SOURCE_TREE}/buildtools/third_party/libc++/trunk/include/")
list(APPEND chromium_external_inc_dirs "${CHROMIUM_SOURCE_TREE}/buildtools/third_party/libc++/")
list(APPEND chromium_external_inc_dirs "${CHROMIUM_SOURCE_TREE}/buildtools/third_party/libc++abi/trunk/include/")

foreach(third abseil-cpp boringssl perfetto protobuf quiche skia)
    list(APPEND chromium_external_inc_dirs "${CHROMIUM_OUT_DIR}/gen/third_party/${third}/build_config")
    list(APPEND chromium_external_inc_dirs "${CHROMIUM_SOURCE_TREE}/third_party/${third}")
    list(APPEND chromium_external_inc_dirs "${CHROMIUM_SOURCE_TREE}/third_party/${third}/include")
    list(APPEND chromium_external_inc_dirs "${CHROMIUM_SOURCE_TREE}/third_party/${third}/src/include")
    list(APPEND chromium_external_inc_dirs "${CHROMIUM_SOURCE_TREE}/third_party/${third}/src")
    list(APPEND chromium_external_inc_dirs "${CHROMIUM_SOURCE_TREE}/net/third_party/${third}/overrides")
    list(APPEND chromium_external_inc_dirs "${CHROMIUM_SOURCE_TREE}/net/third_party/${third}/src")
endforeach()

set(chromium_shared_objects "")
foreach(lib base network_service)
    list(APPEND chromium_shared_objects "${CHROMIUM_OUT_DIR}/lib${lib}.so")
endforeach()