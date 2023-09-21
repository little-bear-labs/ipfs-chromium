########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND protobuf_COMPONENT_NAMES protobuf::libprotobuf protobuf::libprotoc)
list(REMOVE_DUPLICATES protobuf_COMPONENT_NAMES)
list(APPEND protobuf_FIND_DEPENDENCY_NAMES ZLIB)
list(REMOVE_DUPLICATES protobuf_FIND_DEPENDENCY_NAMES)
set(ZLIB_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(protobuf_PACKAGE_FOLDER_DEBUG "/home/runner/.conan2/p/b/protoe27888417002d/p")
set(protobuf_BUILD_MODULES_PATHS_DEBUG "${protobuf_PACKAGE_FOLDER_DEBUG}/lib/cmake/protobuf/protobuf-generate.cmake"
			"${protobuf_PACKAGE_FOLDER_DEBUG}/lib/cmake/protobuf/protobuf-module.cmake"
			"${protobuf_PACKAGE_FOLDER_DEBUG}/lib/cmake/protobuf/protobuf-options.cmake")


set(protobuf_INCLUDE_DIRS_DEBUG "${protobuf_PACKAGE_FOLDER_DEBUG}/include")
set(protobuf_RES_DIRS_DEBUG )
set(protobuf_DEFINITIONS_DEBUG )
set(protobuf_SHARED_LINK_FLAGS_DEBUG )
set(protobuf_EXE_LINK_FLAGS_DEBUG )
set(protobuf_OBJECTS_DEBUG )
set(protobuf_COMPILE_DEFINITIONS_DEBUG )
set(protobuf_COMPILE_OPTIONS_C_DEBUG )
set(protobuf_COMPILE_OPTIONS_CXX_DEBUG )
set(protobuf_LIB_DIRS_DEBUG "${protobuf_PACKAGE_FOLDER_DEBUG}/lib")
set(protobuf_BIN_DIRS_DEBUG )
set(protobuf_LIBRARY_TYPE_DEBUG STATIC)
set(protobuf_IS_HOST_WINDOWS_DEBUG 0)
set(protobuf_LIBS_DEBUG protocd protobufd)
set(protobuf_SYSTEM_LIBS_DEBUG m pthread)
set(protobuf_FRAMEWORK_DIRS_DEBUG )
set(protobuf_FRAMEWORKS_DEBUG )
set(protobuf_BUILD_DIRS_DEBUG "${protobuf_PACKAGE_FOLDER_DEBUG}/lib/cmake/protobuf")
set(protobuf_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(protobuf_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${protobuf_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${protobuf_COMPILE_OPTIONS_C_DEBUG}>")
set(protobuf_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${protobuf_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${protobuf_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${protobuf_EXE_LINK_FLAGS_DEBUG}>")


set(protobuf_COMPONENTS_DEBUG protobuf::libprotobuf protobuf::libprotoc)
########### COMPONENT protobuf::libprotoc VARIABLES ############################################

set(protobuf_protobuf_libprotoc_INCLUDE_DIRS_DEBUG "${protobuf_PACKAGE_FOLDER_DEBUG}/include")
set(protobuf_protobuf_libprotoc_LIB_DIRS_DEBUG "${protobuf_PACKAGE_FOLDER_DEBUG}/lib")
set(protobuf_protobuf_libprotoc_BIN_DIRS_DEBUG )
set(protobuf_protobuf_libprotoc_LIBRARY_TYPE_DEBUG STATIC)
set(protobuf_protobuf_libprotoc_IS_HOST_WINDOWS_DEBUG 0)
set(protobuf_protobuf_libprotoc_RES_DIRS_DEBUG )
set(protobuf_protobuf_libprotoc_DEFINITIONS_DEBUG )
set(protobuf_protobuf_libprotoc_OBJECTS_DEBUG )
set(protobuf_protobuf_libprotoc_COMPILE_DEFINITIONS_DEBUG )
set(protobuf_protobuf_libprotoc_COMPILE_OPTIONS_C_DEBUG "")
set(protobuf_protobuf_libprotoc_COMPILE_OPTIONS_CXX_DEBUG "")
set(protobuf_protobuf_libprotoc_LIBS_DEBUG protocd)
set(protobuf_protobuf_libprotoc_SYSTEM_LIBS_DEBUG )
set(protobuf_protobuf_libprotoc_FRAMEWORK_DIRS_DEBUG )
set(protobuf_protobuf_libprotoc_FRAMEWORKS_DEBUG )
set(protobuf_protobuf_libprotoc_DEPENDENCIES_DEBUG protobuf::libprotobuf)
set(protobuf_protobuf_libprotoc_SHARED_LINK_FLAGS_DEBUG )
set(protobuf_protobuf_libprotoc_EXE_LINK_FLAGS_DEBUG )
set(protobuf_protobuf_libprotoc_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(protobuf_protobuf_libprotoc_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${protobuf_protobuf_libprotoc_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${protobuf_protobuf_libprotoc_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${protobuf_protobuf_libprotoc_EXE_LINK_FLAGS_DEBUG}>
)
set(protobuf_protobuf_libprotoc_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${protobuf_protobuf_libprotoc_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${protobuf_protobuf_libprotoc_COMPILE_OPTIONS_C_DEBUG}>")
########### COMPONENT protobuf::libprotobuf VARIABLES ############################################

set(protobuf_protobuf_libprotobuf_INCLUDE_DIRS_DEBUG "${protobuf_PACKAGE_FOLDER_DEBUG}/include")
set(protobuf_protobuf_libprotobuf_LIB_DIRS_DEBUG "${protobuf_PACKAGE_FOLDER_DEBUG}/lib")
set(protobuf_protobuf_libprotobuf_BIN_DIRS_DEBUG )
set(protobuf_protobuf_libprotobuf_LIBRARY_TYPE_DEBUG STATIC)
set(protobuf_protobuf_libprotobuf_IS_HOST_WINDOWS_DEBUG 0)
set(protobuf_protobuf_libprotobuf_RES_DIRS_DEBUG )
set(protobuf_protobuf_libprotobuf_DEFINITIONS_DEBUG )
set(protobuf_protobuf_libprotobuf_OBJECTS_DEBUG )
set(protobuf_protobuf_libprotobuf_COMPILE_DEFINITIONS_DEBUG )
set(protobuf_protobuf_libprotobuf_COMPILE_OPTIONS_C_DEBUG "")
set(protobuf_protobuf_libprotobuf_COMPILE_OPTIONS_CXX_DEBUG "")
set(protobuf_protobuf_libprotobuf_LIBS_DEBUG protobufd)
set(protobuf_protobuf_libprotobuf_SYSTEM_LIBS_DEBUG m pthread)
set(protobuf_protobuf_libprotobuf_FRAMEWORK_DIRS_DEBUG )
set(protobuf_protobuf_libprotobuf_FRAMEWORKS_DEBUG )
set(protobuf_protobuf_libprotobuf_DEPENDENCIES_DEBUG ZLIB::ZLIB)
set(protobuf_protobuf_libprotobuf_SHARED_LINK_FLAGS_DEBUG )
set(protobuf_protobuf_libprotobuf_EXE_LINK_FLAGS_DEBUG )
set(protobuf_protobuf_libprotobuf_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(protobuf_protobuf_libprotobuf_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${protobuf_protobuf_libprotobuf_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${protobuf_protobuf_libprotobuf_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${protobuf_protobuf_libprotobuf_EXE_LINK_FLAGS_DEBUG}>
)
set(protobuf_protobuf_libprotobuf_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${protobuf_protobuf_libprotobuf_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${protobuf_protobuf_libprotobuf_COMPILE_OPTIONS_C_DEBUG}>")