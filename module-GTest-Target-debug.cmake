# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(gtest_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(gtest_FRAMEWORKS_FOUND_DEBUG "${gtest_FRAMEWORKS_DEBUG}" "${gtest_FRAMEWORK_DIRS_DEBUG}")

set(gtest_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET gtest_DEPS_TARGET)
    add_library(gtest_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET gtest_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${gtest_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${gtest_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:GTest::gtest;GTest::gmock>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### gtest_DEPS_TARGET to all of them
conan_package_library_targets("${gtest_LIBS_DEBUG}"    # libraries
                              "${gtest_LIB_DIRS_DEBUG}" # package_libdir
                              "${gtest_BIN_DIRS_DEBUG}" # package_bindir
                              "${gtest_LIBRARY_TYPE_DEBUG}"
                              "${gtest_IS_HOST_WINDOWS_DEBUG}"
                              gtest_DEPS_TARGET
                              gtest_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "gtest"    # package_name
                              "${gtest_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${gtest_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT GTest::gmock_main #############

        set(gtest_GTest_gmock_main_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(gtest_GTest_gmock_main_FRAMEWORKS_FOUND_DEBUG "${gtest_GTest_gmock_main_FRAMEWORKS_DEBUG}" "${gtest_GTest_gmock_main_FRAMEWORK_DIRS_DEBUG}")

        set(gtest_GTest_gmock_main_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET gtest_GTest_gmock_main_DEPS_TARGET)
            add_library(gtest_GTest_gmock_main_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET gtest_GTest_gmock_main_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_main_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_main_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_main_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'gtest_GTest_gmock_main_DEPS_TARGET' to all of them
        conan_package_library_targets("${gtest_GTest_gmock_main_LIBS_DEBUG}"
                              "${gtest_GTest_gmock_main_LIB_DIRS_DEBUG}"
                              "${gtest_GTest_gmock_main_BIN_DIRS_DEBUG}" # package_bindir
                              "${gtest_GTest_gmock_main_LIBRARY_TYPE_DEBUG}"
                              "${gtest_GTest_gmock_main_IS_HOST_WINDOWS_DEBUG}"
                              gtest_GTest_gmock_main_DEPS_TARGET
                              gtest_GTest_gmock_main_LIBRARIES_TARGETS
                              "_DEBUG"
                              "gtest_GTest_gmock_main"
                              "${gtest_GTest_gmock_main_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET GTest::gmock_main
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_main_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_main_LIBRARIES_TARGETS}>
                     APPEND)

        if("${gtest_GTest_gmock_main_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET GTest::gmock_main
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         gtest_GTest_gmock_main_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET GTest::gmock_main PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_main_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET GTest::gmock_main PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_main_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET GTest::gmock_main PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_main_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET GTest::gmock_main PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_main_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET GTest::gmock_main PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_main_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT GTest::gmock #############

        set(gtest_GTest_gmock_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(gtest_GTest_gmock_FRAMEWORKS_FOUND_DEBUG "${gtest_GTest_gmock_FRAMEWORKS_DEBUG}" "${gtest_GTest_gmock_FRAMEWORK_DIRS_DEBUG}")

        set(gtest_GTest_gmock_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET gtest_GTest_gmock_DEPS_TARGET)
            add_library(gtest_GTest_gmock_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET gtest_GTest_gmock_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'gtest_GTest_gmock_DEPS_TARGET' to all of them
        conan_package_library_targets("${gtest_GTest_gmock_LIBS_DEBUG}"
                              "${gtest_GTest_gmock_LIB_DIRS_DEBUG}"
                              "${gtest_GTest_gmock_BIN_DIRS_DEBUG}" # package_bindir
                              "${gtest_GTest_gmock_LIBRARY_TYPE_DEBUG}"
                              "${gtest_GTest_gmock_IS_HOST_WINDOWS_DEBUG}"
                              gtest_GTest_gmock_DEPS_TARGET
                              gtest_GTest_gmock_LIBRARIES_TARGETS
                              "_DEBUG"
                              "gtest_GTest_gmock"
                              "${gtest_GTest_gmock_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET GTest::gmock
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_LIBRARIES_TARGETS}>
                     APPEND)

        if("${gtest_GTest_gmock_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET GTest::gmock
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         gtest_GTest_gmock_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET GTest::gmock PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET GTest::gmock PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET GTest::gmock PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET GTest::gmock PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET GTest::gmock PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gmock_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT GTest::gtest_main #############

        set(gtest_GTest_gtest_main_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(gtest_GTest_gtest_main_FRAMEWORKS_FOUND_DEBUG "${gtest_GTest_gtest_main_FRAMEWORKS_DEBUG}" "${gtest_GTest_gtest_main_FRAMEWORK_DIRS_DEBUG}")

        set(gtest_GTest_gtest_main_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET gtest_GTest_gtest_main_DEPS_TARGET)
            add_library(gtest_GTest_gtest_main_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET gtest_GTest_gtest_main_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_main_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_main_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_main_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'gtest_GTest_gtest_main_DEPS_TARGET' to all of them
        conan_package_library_targets("${gtest_GTest_gtest_main_LIBS_DEBUG}"
                              "${gtest_GTest_gtest_main_LIB_DIRS_DEBUG}"
                              "${gtest_GTest_gtest_main_BIN_DIRS_DEBUG}" # package_bindir
                              "${gtest_GTest_gtest_main_LIBRARY_TYPE_DEBUG}"
                              "${gtest_GTest_gtest_main_IS_HOST_WINDOWS_DEBUG}"
                              gtest_GTest_gtest_main_DEPS_TARGET
                              gtest_GTest_gtest_main_LIBRARIES_TARGETS
                              "_DEBUG"
                              "gtest_GTest_gtest_main"
                              "${gtest_GTest_gtest_main_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET GTest::gtest_main
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_main_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_main_LIBRARIES_TARGETS}>
                     APPEND)

        if("${gtest_GTest_gtest_main_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET GTest::gtest_main
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         gtest_GTest_gtest_main_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET GTest::gtest_main PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_main_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET GTest::gtest_main PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_main_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET GTest::gtest_main PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_main_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET GTest::gtest_main PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_main_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET GTest::gtest_main PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_main_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT GTest::gtest #############

        set(gtest_GTest_gtest_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(gtest_GTest_gtest_FRAMEWORKS_FOUND_DEBUG "${gtest_GTest_gtest_FRAMEWORKS_DEBUG}" "${gtest_GTest_gtest_FRAMEWORK_DIRS_DEBUG}")

        set(gtest_GTest_gtest_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET gtest_GTest_gtest_DEPS_TARGET)
            add_library(gtest_GTest_gtest_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET gtest_GTest_gtest_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'gtest_GTest_gtest_DEPS_TARGET' to all of them
        conan_package_library_targets("${gtest_GTest_gtest_LIBS_DEBUG}"
                              "${gtest_GTest_gtest_LIB_DIRS_DEBUG}"
                              "${gtest_GTest_gtest_BIN_DIRS_DEBUG}" # package_bindir
                              "${gtest_GTest_gtest_LIBRARY_TYPE_DEBUG}"
                              "${gtest_GTest_gtest_IS_HOST_WINDOWS_DEBUG}"
                              gtest_GTest_gtest_DEPS_TARGET
                              gtest_GTest_gtest_LIBRARIES_TARGETS
                              "_DEBUG"
                              "gtest_GTest_gtest"
                              "${gtest_GTest_gtest_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET GTest::gtest
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_LIBRARIES_TARGETS}>
                     APPEND)

        if("${gtest_GTest_gtest_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET GTest::gtest
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         gtest_GTest_gtest_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET GTest::gtest PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET GTest::gtest PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET GTest::gtest PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET GTest::gtest PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET GTest::gtest PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${gtest_GTest_gtest_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET gtest::gtest PROPERTY INTERFACE_LINK_LIBRARIES GTest::gmock_main APPEND)
    set_property(TARGET gtest::gtest PROPERTY INTERFACE_LINK_LIBRARIES GTest::gmock APPEND)
    set_property(TARGET gtest::gtest PROPERTY INTERFACE_LINK_LIBRARIES GTest::gtest_main APPEND)
    set_property(TARGET gtest::gtest PROPERTY INTERFACE_LINK_LIBRARIES GTest::gtest APPEND)

########## For the modules (FindXXX)
set(gtest_LIBRARIES_DEBUG gtest::gtest)
