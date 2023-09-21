# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(boost_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(boost_FRAMEWORKS_FOUND_DEBUG "${boost_FRAMEWORKS_DEBUG}" "${boost_FRAMEWORK_DIRS_DEBUG}")

set(boost_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET boost_DEPS_TARGET)
    add_library(boost_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET boost_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${boost_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:Boost::headers>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### boost_DEPS_TARGET to all of them
conan_package_library_targets("${boost_LIBS_DEBUG}"    # libraries
                              "${boost_LIB_DIRS_DEBUG}" # package_libdir
                              "${boost_BIN_DIRS_DEBUG}" # package_bindir
                              "${boost_LIBRARY_TYPE_DEBUG}"
                              "${boost_IS_HOST_WINDOWS_DEBUG}"
                              boost_DEPS_TARGET
                              boost_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "boost"    # package_name
                              "${boost_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${boost_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT Boost::boost #############

        set(boost_Boost_boost_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(boost_Boost_boost_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_boost_FRAMEWORKS_DEBUG}" "${boost_Boost_boost_FRAMEWORK_DIRS_DEBUG}")

        set(boost_Boost_boost_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET boost_Boost_boost_DEPS_TARGET)
            add_library(boost_Boost_boost_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET boost_Boost_boost_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${boost_Boost_boost_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${boost_Boost_boost_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${boost_Boost_boost_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'boost_Boost_boost_DEPS_TARGET' to all of them
        conan_package_library_targets("${boost_Boost_boost_LIBS_DEBUG}"
                              "${boost_Boost_boost_LIB_DIRS_DEBUG}"
                              "${boost_Boost_boost_BIN_DIRS_DEBUG}" # package_bindir
                              "${boost_Boost_boost_LIBRARY_TYPE_DEBUG}"
                              "${boost_Boost_boost_IS_HOST_WINDOWS_DEBUG}"
                              boost_Boost_boost_DEPS_TARGET
                              boost_Boost_boost_LIBRARIES_TARGETS
                              "_DEBUG"
                              "boost_Boost_boost"
                              "${boost_Boost_boost_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET Boost::boost
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${boost_Boost_boost_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${boost_Boost_boost_LIBRARIES_TARGETS}>
                     APPEND)

        if("${boost_Boost_boost_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET Boost::boost
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         boost_Boost_boost_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET Boost::boost PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${boost_Boost_boost_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET Boost::boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${boost_Boost_boost_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET Boost::boost PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${boost_Boost_boost_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET Boost::boost PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${boost_Boost_boost_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET Boost::boost PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${boost_Boost_boost_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT Boost::headers #############

        set(boost_Boost_headers_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(boost_Boost_headers_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_headers_FRAMEWORKS_DEBUG}" "${boost_Boost_headers_FRAMEWORK_DIRS_DEBUG}")

        set(boost_Boost_headers_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET boost_Boost_headers_DEPS_TARGET)
            add_library(boost_Boost_headers_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET boost_Boost_headers_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${boost_Boost_headers_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${boost_Boost_headers_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${boost_Boost_headers_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'boost_Boost_headers_DEPS_TARGET' to all of them
        conan_package_library_targets("${boost_Boost_headers_LIBS_DEBUG}"
                              "${boost_Boost_headers_LIB_DIRS_DEBUG}"
                              "${boost_Boost_headers_BIN_DIRS_DEBUG}" # package_bindir
                              "${boost_Boost_headers_LIBRARY_TYPE_DEBUG}"
                              "${boost_Boost_headers_IS_HOST_WINDOWS_DEBUG}"
                              boost_Boost_headers_DEPS_TARGET
                              boost_Boost_headers_LIBRARIES_TARGETS
                              "_DEBUG"
                              "boost_Boost_headers"
                              "${boost_Boost_headers_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET Boost::headers
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${boost_Boost_headers_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${boost_Boost_headers_LIBRARIES_TARGETS}>
                     APPEND)

        if("${boost_Boost_headers_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET Boost::headers
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         boost_Boost_headers_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET Boost::headers PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${boost_Boost_headers_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET Boost::headers PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${boost_Boost_headers_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET Boost::headers PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${boost_Boost_headers_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET Boost::headers PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${boost_Boost_headers_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET Boost::headers PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${boost_Boost_headers_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET boost::boost PROPERTY INTERFACE_LINK_LIBRARIES Boost::boost APPEND)
    set_property(TARGET boost::boost PROPERTY INTERFACE_LINK_LIBRARIES Boost::headers APPEND)

########## For the modules (FindXXX)
set(boost_LIBRARIES_DEBUG boost::boost)
