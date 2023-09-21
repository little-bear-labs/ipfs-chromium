########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND boost_COMPONENT_NAMES Boost::headers Boost::boost)
list(REMOVE_DUPLICATES boost_COMPONENT_NAMES)
set(boost_FIND_DEPENDENCY_NAMES "")

########### VARIABLES #######################################################################
#############################################################################################
set(boost_PACKAGE_FOLDER_DEBUG "/home/runner/.conan2/p/boostfffbedcd6bfa8/p")
set(boost_BUILD_MODULES_PATHS_DEBUG )


set(boost_INCLUDE_DIRS_DEBUG "${boost_PACKAGE_FOLDER_DEBUG}/include")
set(boost_RES_DIRS_DEBUG )
set(boost_DEFINITIONS_DEBUG )
set(boost_SHARED_LINK_FLAGS_DEBUG )
set(boost_EXE_LINK_FLAGS_DEBUG )
set(boost_OBJECTS_DEBUG )
set(boost_COMPILE_DEFINITIONS_DEBUG )
set(boost_COMPILE_OPTIONS_C_DEBUG )
set(boost_COMPILE_OPTIONS_CXX_DEBUG )
set(boost_LIB_DIRS_DEBUG )
set(boost_BIN_DIRS_DEBUG )
set(boost_LIBRARY_TYPE_DEBUG UNKNOWN)
set(boost_IS_HOST_WINDOWS_DEBUG 0)
set(boost_LIBS_DEBUG )
set(boost_SYSTEM_LIBS_DEBUG )
set(boost_FRAMEWORK_DIRS_DEBUG )
set(boost_FRAMEWORKS_DEBUG )
set(boost_BUILD_DIRS_DEBUG )
set(boost_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(boost_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${boost_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${boost_COMPILE_OPTIONS_C_DEBUG}>")
set(boost_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${boost_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${boost_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${boost_EXE_LINK_FLAGS_DEBUG}>")


set(boost_COMPONENTS_DEBUG Boost::headers Boost::boost)
########### COMPONENT Boost::boost VARIABLES ############################################

set(boost_Boost_boost_INCLUDE_DIRS_DEBUG "${boost_PACKAGE_FOLDER_DEBUG}/include")
set(boost_Boost_boost_LIB_DIRS_DEBUG )
set(boost_Boost_boost_BIN_DIRS_DEBUG )
set(boost_Boost_boost_LIBRARY_TYPE_DEBUG UNKNOWN)
set(boost_Boost_boost_IS_HOST_WINDOWS_DEBUG 0)
set(boost_Boost_boost_RES_DIRS_DEBUG )
set(boost_Boost_boost_DEFINITIONS_DEBUG )
set(boost_Boost_boost_OBJECTS_DEBUG )
set(boost_Boost_boost_COMPILE_DEFINITIONS_DEBUG )
set(boost_Boost_boost_COMPILE_OPTIONS_C_DEBUG "")
set(boost_Boost_boost_COMPILE_OPTIONS_CXX_DEBUG "")
set(boost_Boost_boost_LIBS_DEBUG )
set(boost_Boost_boost_SYSTEM_LIBS_DEBUG )
set(boost_Boost_boost_FRAMEWORK_DIRS_DEBUG )
set(boost_Boost_boost_FRAMEWORKS_DEBUG )
set(boost_Boost_boost_DEPENDENCIES_DEBUG Boost::headers)
set(boost_Boost_boost_SHARED_LINK_FLAGS_DEBUG )
set(boost_Boost_boost_EXE_LINK_FLAGS_DEBUG )
set(boost_Boost_boost_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(boost_Boost_boost_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${boost_Boost_boost_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${boost_Boost_boost_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${boost_Boost_boost_EXE_LINK_FLAGS_DEBUG}>
)
set(boost_Boost_boost_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${boost_Boost_boost_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${boost_Boost_boost_COMPILE_OPTIONS_C_DEBUG}>")
########### COMPONENT Boost::headers VARIABLES ############################################

set(boost_Boost_headers_INCLUDE_DIRS_DEBUG "${boost_PACKAGE_FOLDER_DEBUG}/include")
set(boost_Boost_headers_LIB_DIRS_DEBUG )
set(boost_Boost_headers_BIN_DIRS_DEBUG )
set(boost_Boost_headers_LIBRARY_TYPE_DEBUG UNKNOWN)
set(boost_Boost_headers_IS_HOST_WINDOWS_DEBUG 0)
set(boost_Boost_headers_RES_DIRS_DEBUG )
set(boost_Boost_headers_DEFINITIONS_DEBUG )
set(boost_Boost_headers_OBJECTS_DEBUG )
set(boost_Boost_headers_COMPILE_DEFINITIONS_DEBUG )
set(boost_Boost_headers_COMPILE_OPTIONS_C_DEBUG "")
set(boost_Boost_headers_COMPILE_OPTIONS_CXX_DEBUG "")
set(boost_Boost_headers_LIBS_DEBUG )
set(boost_Boost_headers_SYSTEM_LIBS_DEBUG )
set(boost_Boost_headers_FRAMEWORK_DIRS_DEBUG )
set(boost_Boost_headers_FRAMEWORKS_DEBUG )
set(boost_Boost_headers_DEPENDENCIES_DEBUG )
set(boost_Boost_headers_SHARED_LINK_FLAGS_DEBUG )
set(boost_Boost_headers_EXE_LINK_FLAGS_DEBUG )
set(boost_Boost_headers_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(boost_Boost_headers_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${boost_Boost_headers_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${boost_Boost_headers_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${boost_Boost_headers_EXE_LINK_FLAGS_DEBUG}>
)
set(boost_Boost_headers_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${boost_Boost_headers_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${boost_Boost_headers_COMPILE_OPTIONS_C_DEBUG}>")