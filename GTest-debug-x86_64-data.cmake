########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND gtest_COMPONENT_NAMES GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
list(REMOVE_DUPLICATES gtest_COMPONENT_NAMES)
set(gtest_FIND_DEPENDENCY_NAMES "")

########### VARIABLES #######################################################################
#############################################################################################
set(gtest_PACKAGE_FOLDER_DEBUG "/home/runner/.conan2/p/b/gtest1652482968105/p")
set(gtest_BUILD_MODULES_PATHS_DEBUG )


set(gtest_INCLUDE_DIRS_DEBUG "${gtest_PACKAGE_FOLDER_DEBUG}/include")
set(gtest_RES_DIRS_DEBUG )
set(gtest_DEFINITIONS_DEBUG )
set(gtest_SHARED_LINK_FLAGS_DEBUG )
set(gtest_EXE_LINK_FLAGS_DEBUG )
set(gtest_OBJECTS_DEBUG )
set(gtest_COMPILE_DEFINITIONS_DEBUG )
set(gtest_COMPILE_OPTIONS_C_DEBUG )
set(gtest_COMPILE_OPTIONS_CXX_DEBUG )
set(gtest_LIB_DIRS_DEBUG "${gtest_PACKAGE_FOLDER_DEBUG}/lib")
set(gtest_BIN_DIRS_DEBUG )
set(gtest_LIBRARY_TYPE_DEBUG STATIC)
set(gtest_IS_HOST_WINDOWS_DEBUG 0)
set(gtest_LIBS_DEBUG gmock_main gmock gtest_main gtest)
set(gtest_SYSTEM_LIBS_DEBUG m pthread)
set(gtest_FRAMEWORK_DIRS_DEBUG )
set(gtest_FRAMEWORKS_DEBUG )
set(gtest_BUILD_DIRS_DEBUG )
set(gtest_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(gtest_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${gtest_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${gtest_COMPILE_OPTIONS_C_DEBUG}>")
set(gtest_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${gtest_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${gtest_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${gtest_EXE_LINK_FLAGS_DEBUG}>")


set(gtest_COMPONENTS_DEBUG GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
########### COMPONENT GTest::gmock_main VARIABLES ############################################

set(gtest_GTest_gmock_main_INCLUDE_DIRS_DEBUG "${gtest_PACKAGE_FOLDER_DEBUG}/include")
set(gtest_GTest_gmock_main_LIB_DIRS_DEBUG "${gtest_PACKAGE_FOLDER_DEBUG}/lib")
set(gtest_GTest_gmock_main_BIN_DIRS_DEBUG )
set(gtest_GTest_gmock_main_LIBRARY_TYPE_DEBUG STATIC)
set(gtest_GTest_gmock_main_IS_HOST_WINDOWS_DEBUG 0)
set(gtest_GTest_gmock_main_RES_DIRS_DEBUG )
set(gtest_GTest_gmock_main_DEFINITIONS_DEBUG )
set(gtest_GTest_gmock_main_OBJECTS_DEBUG )
set(gtest_GTest_gmock_main_COMPILE_DEFINITIONS_DEBUG )
set(gtest_GTest_gmock_main_COMPILE_OPTIONS_C_DEBUG "")
set(gtest_GTest_gmock_main_COMPILE_OPTIONS_CXX_DEBUG "")
set(gtest_GTest_gmock_main_LIBS_DEBUG gmock_main)
set(gtest_GTest_gmock_main_SYSTEM_LIBS_DEBUG )
set(gtest_GTest_gmock_main_FRAMEWORK_DIRS_DEBUG )
set(gtest_GTest_gmock_main_FRAMEWORKS_DEBUG )
set(gtest_GTest_gmock_main_DEPENDENCIES_DEBUG GTest::gmock)
set(gtest_GTest_gmock_main_SHARED_LINK_FLAGS_DEBUG )
set(gtest_GTest_gmock_main_EXE_LINK_FLAGS_DEBUG )
set(gtest_GTest_gmock_main_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(gtest_GTest_gmock_main_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${gtest_GTest_gmock_main_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${gtest_GTest_gmock_main_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${gtest_GTest_gmock_main_EXE_LINK_FLAGS_DEBUG}>
)
set(gtest_GTest_gmock_main_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${gtest_GTest_gmock_main_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${gtest_GTest_gmock_main_COMPILE_OPTIONS_C_DEBUG}>")
########### COMPONENT GTest::gmock VARIABLES ############################################

set(gtest_GTest_gmock_INCLUDE_DIRS_DEBUG "${gtest_PACKAGE_FOLDER_DEBUG}/include")
set(gtest_GTest_gmock_LIB_DIRS_DEBUG "${gtest_PACKAGE_FOLDER_DEBUG}/lib")
set(gtest_GTest_gmock_BIN_DIRS_DEBUG )
set(gtest_GTest_gmock_LIBRARY_TYPE_DEBUG STATIC)
set(gtest_GTest_gmock_IS_HOST_WINDOWS_DEBUG 0)
set(gtest_GTest_gmock_RES_DIRS_DEBUG )
set(gtest_GTest_gmock_DEFINITIONS_DEBUG )
set(gtest_GTest_gmock_OBJECTS_DEBUG )
set(gtest_GTest_gmock_COMPILE_DEFINITIONS_DEBUG )
set(gtest_GTest_gmock_COMPILE_OPTIONS_C_DEBUG "")
set(gtest_GTest_gmock_COMPILE_OPTIONS_CXX_DEBUG "")
set(gtest_GTest_gmock_LIBS_DEBUG gmock)
set(gtest_GTest_gmock_SYSTEM_LIBS_DEBUG )
set(gtest_GTest_gmock_FRAMEWORK_DIRS_DEBUG )
set(gtest_GTest_gmock_FRAMEWORKS_DEBUG )
set(gtest_GTest_gmock_DEPENDENCIES_DEBUG GTest::gtest)
set(gtest_GTest_gmock_SHARED_LINK_FLAGS_DEBUG )
set(gtest_GTest_gmock_EXE_LINK_FLAGS_DEBUG )
set(gtest_GTest_gmock_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(gtest_GTest_gmock_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${gtest_GTest_gmock_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${gtest_GTest_gmock_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${gtest_GTest_gmock_EXE_LINK_FLAGS_DEBUG}>
)
set(gtest_GTest_gmock_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${gtest_GTest_gmock_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${gtest_GTest_gmock_COMPILE_OPTIONS_C_DEBUG}>")
########### COMPONENT GTest::gtest_main VARIABLES ############################################

set(gtest_GTest_gtest_main_INCLUDE_DIRS_DEBUG "${gtest_PACKAGE_FOLDER_DEBUG}/include")
set(gtest_GTest_gtest_main_LIB_DIRS_DEBUG "${gtest_PACKAGE_FOLDER_DEBUG}/lib")
set(gtest_GTest_gtest_main_BIN_DIRS_DEBUG )
set(gtest_GTest_gtest_main_LIBRARY_TYPE_DEBUG STATIC)
set(gtest_GTest_gtest_main_IS_HOST_WINDOWS_DEBUG 0)
set(gtest_GTest_gtest_main_RES_DIRS_DEBUG )
set(gtest_GTest_gtest_main_DEFINITIONS_DEBUG )
set(gtest_GTest_gtest_main_OBJECTS_DEBUG )
set(gtest_GTest_gtest_main_COMPILE_DEFINITIONS_DEBUG )
set(gtest_GTest_gtest_main_COMPILE_OPTIONS_C_DEBUG "")
set(gtest_GTest_gtest_main_COMPILE_OPTIONS_CXX_DEBUG "")
set(gtest_GTest_gtest_main_LIBS_DEBUG gtest_main)
set(gtest_GTest_gtest_main_SYSTEM_LIBS_DEBUG )
set(gtest_GTest_gtest_main_FRAMEWORK_DIRS_DEBUG )
set(gtest_GTest_gtest_main_FRAMEWORKS_DEBUG )
set(gtest_GTest_gtest_main_DEPENDENCIES_DEBUG GTest::gtest)
set(gtest_GTest_gtest_main_SHARED_LINK_FLAGS_DEBUG )
set(gtest_GTest_gtest_main_EXE_LINK_FLAGS_DEBUG )
set(gtest_GTest_gtest_main_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(gtest_GTest_gtest_main_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${gtest_GTest_gtest_main_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${gtest_GTest_gtest_main_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${gtest_GTest_gtest_main_EXE_LINK_FLAGS_DEBUG}>
)
set(gtest_GTest_gtest_main_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${gtest_GTest_gtest_main_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${gtest_GTest_gtest_main_COMPILE_OPTIONS_C_DEBUG}>")
########### COMPONENT GTest::gtest VARIABLES ############################################

set(gtest_GTest_gtest_INCLUDE_DIRS_DEBUG "${gtest_PACKAGE_FOLDER_DEBUG}/include")
set(gtest_GTest_gtest_LIB_DIRS_DEBUG "${gtest_PACKAGE_FOLDER_DEBUG}/lib")
set(gtest_GTest_gtest_BIN_DIRS_DEBUG )
set(gtest_GTest_gtest_LIBRARY_TYPE_DEBUG STATIC)
set(gtest_GTest_gtest_IS_HOST_WINDOWS_DEBUG 0)
set(gtest_GTest_gtest_RES_DIRS_DEBUG )
set(gtest_GTest_gtest_DEFINITIONS_DEBUG )
set(gtest_GTest_gtest_OBJECTS_DEBUG )
set(gtest_GTest_gtest_COMPILE_DEFINITIONS_DEBUG )
set(gtest_GTest_gtest_COMPILE_OPTIONS_C_DEBUG "")
set(gtest_GTest_gtest_COMPILE_OPTIONS_CXX_DEBUG "")
set(gtest_GTest_gtest_LIBS_DEBUG gtest)
set(gtest_GTest_gtest_SYSTEM_LIBS_DEBUG m pthread)
set(gtest_GTest_gtest_FRAMEWORK_DIRS_DEBUG )
set(gtest_GTest_gtest_FRAMEWORKS_DEBUG )
set(gtest_GTest_gtest_DEPENDENCIES_DEBUG )
set(gtest_GTest_gtest_SHARED_LINK_FLAGS_DEBUG )
set(gtest_GTest_gtest_EXE_LINK_FLAGS_DEBUG )
set(gtest_GTest_gtest_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(gtest_GTest_gtest_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${gtest_GTest_gtest_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${gtest_GTest_gtest_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${gtest_GTest_gtest_EXE_LINK_FLAGS_DEBUG}>
)
set(gtest_GTest_gtest_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${gtest_GTest_gtest_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${gtest_GTest_gtest_COMPILE_OPTIONS_C_DEBUG}>")