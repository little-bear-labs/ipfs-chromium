# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(openssl_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(openssl_FRAMEWORKS_FOUND_DEBUG "${openssl_FRAMEWORKS_DEBUG}" "${openssl_FRAMEWORK_DIRS_DEBUG}")

set(openssl_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET openssl_DEPS_TARGET)
    add_library(openssl_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET openssl_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${openssl_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${openssl_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:OpenSSL::Crypto>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### openssl_DEPS_TARGET to all of them
conan_package_library_targets("${openssl_LIBS_DEBUG}"    # libraries
                              "${openssl_LIB_DIRS_DEBUG}" # package_libdir
                              "${openssl_BIN_DIRS_DEBUG}" # package_bindir
                              "${openssl_LIBRARY_TYPE_DEBUG}"
                              "${openssl_IS_HOST_WINDOWS_DEBUG}"
                              openssl_DEPS_TARGET
                              openssl_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "openssl"    # package_name
                              "${openssl_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${openssl_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT OpenSSL::SSL #############

        set(openssl_OpenSSL_SSL_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(openssl_OpenSSL_SSL_FRAMEWORKS_FOUND_DEBUG "${openssl_OpenSSL_SSL_FRAMEWORKS_DEBUG}" "${openssl_OpenSSL_SSL_FRAMEWORK_DIRS_DEBUG}")

        set(openssl_OpenSSL_SSL_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET openssl_OpenSSL_SSL_DEPS_TARGET)
            add_library(openssl_OpenSSL_SSL_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET openssl_OpenSSL_SSL_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'openssl_OpenSSL_SSL_DEPS_TARGET' to all of them
        conan_package_library_targets("${openssl_OpenSSL_SSL_LIBS_DEBUG}"
                              "${openssl_OpenSSL_SSL_LIB_DIRS_DEBUG}"
                              "${openssl_OpenSSL_SSL_BIN_DIRS_DEBUG}" # package_bindir
                              "${openssl_OpenSSL_SSL_LIBRARY_TYPE_DEBUG}"
                              "${openssl_OpenSSL_SSL_IS_HOST_WINDOWS_DEBUG}"
                              openssl_OpenSSL_SSL_DEPS_TARGET
                              openssl_OpenSSL_SSL_LIBRARIES_TARGETS
                              "_DEBUG"
                              "openssl_OpenSSL_SSL"
                              "${openssl_OpenSSL_SSL_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET OpenSSL::SSL
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_LIBRARIES_TARGETS}>
                     APPEND)

        if("${openssl_OpenSSL_SSL_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET OpenSSL::SSL
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         openssl_OpenSSL_SSL_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT OpenSSL::Crypto #############

        set(openssl_OpenSSL_Crypto_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(openssl_OpenSSL_Crypto_FRAMEWORKS_FOUND_DEBUG "${openssl_OpenSSL_Crypto_FRAMEWORKS_DEBUG}" "${openssl_OpenSSL_Crypto_FRAMEWORK_DIRS_DEBUG}")

        set(openssl_OpenSSL_Crypto_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET openssl_OpenSSL_Crypto_DEPS_TARGET)
            add_library(openssl_OpenSSL_Crypto_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET openssl_OpenSSL_Crypto_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'openssl_OpenSSL_Crypto_DEPS_TARGET' to all of them
        conan_package_library_targets("${openssl_OpenSSL_Crypto_LIBS_DEBUG}"
                              "${openssl_OpenSSL_Crypto_LIB_DIRS_DEBUG}"
                              "${openssl_OpenSSL_Crypto_BIN_DIRS_DEBUG}" # package_bindir
                              "${openssl_OpenSSL_Crypto_LIBRARY_TYPE_DEBUG}"
                              "${openssl_OpenSSL_Crypto_IS_HOST_WINDOWS_DEBUG}"
                              openssl_OpenSSL_Crypto_DEPS_TARGET
                              openssl_OpenSSL_Crypto_LIBRARIES_TARGETS
                              "_DEBUG"
                              "openssl_OpenSSL_Crypto"
                              "${openssl_OpenSSL_Crypto_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET OpenSSL::Crypto
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_LIBRARIES_TARGETS}>
                     APPEND)

        if("${openssl_OpenSSL_Crypto_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET OpenSSL::Crypto
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         openssl_OpenSSL_Crypto_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET openssl::openssl PROPERTY INTERFACE_LINK_LIBRARIES OpenSSL::SSL APPEND)
    set_property(TARGET openssl::openssl PROPERTY INTERFACE_LINK_LIBRARIES OpenSSL::Crypto APPEND)

########## For the modules (FindXXX)
set(openssl_LIBRARIES_DEBUG openssl::openssl)
