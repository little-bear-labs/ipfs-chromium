########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND openssl_COMPONENT_NAMES OpenSSL::Crypto OpenSSL::SSL)
list(REMOVE_DUPLICATES openssl_COMPONENT_NAMES)
set(openssl_FIND_DEPENDENCY_NAMES "")

########### VARIABLES #######################################################################
#############################################################################################
set(openssl_PACKAGE_FOLDER_DEBUG "/home/runner/.conan2/p/b/opens27a27c4075b9c/p")
set(openssl_BUILD_MODULES_PATHS_DEBUG "${openssl_PACKAGE_FOLDER_DEBUG}/lib/cmake/conan-official-openssl-variables.cmake")


set(openssl_INCLUDE_DIRS_DEBUG "${openssl_PACKAGE_FOLDER_DEBUG}/include")
set(openssl_RES_DIRS_DEBUG )
set(openssl_DEFINITIONS_DEBUG )
set(openssl_SHARED_LINK_FLAGS_DEBUG )
set(openssl_EXE_LINK_FLAGS_DEBUG )
set(openssl_OBJECTS_DEBUG )
set(openssl_COMPILE_DEFINITIONS_DEBUG )
set(openssl_COMPILE_OPTIONS_C_DEBUG )
set(openssl_COMPILE_OPTIONS_CXX_DEBUG )
set(openssl_LIB_DIRS_DEBUG "${openssl_PACKAGE_FOLDER_DEBUG}/lib")
set(openssl_BIN_DIRS_DEBUG )
set(openssl_LIBRARY_TYPE_DEBUG STATIC)
set(openssl_IS_HOST_WINDOWS_DEBUG 0)
set(openssl_LIBS_DEBUG ssl crypto)
set(openssl_SYSTEM_LIBS_DEBUG dl pthread rt)
set(openssl_FRAMEWORK_DIRS_DEBUG )
set(openssl_FRAMEWORKS_DEBUG )
set(openssl_BUILD_DIRS_DEBUG )
set(openssl_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(openssl_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${openssl_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${openssl_COMPILE_OPTIONS_C_DEBUG}>")
set(openssl_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${openssl_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${openssl_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${openssl_EXE_LINK_FLAGS_DEBUG}>")


set(openssl_COMPONENTS_DEBUG OpenSSL::Crypto OpenSSL::SSL)
########### COMPONENT OpenSSL::SSL VARIABLES ############################################

set(openssl_OpenSSL_SSL_INCLUDE_DIRS_DEBUG "${openssl_PACKAGE_FOLDER_DEBUG}/include")
set(openssl_OpenSSL_SSL_LIB_DIRS_DEBUG "${openssl_PACKAGE_FOLDER_DEBUG}/lib")
set(openssl_OpenSSL_SSL_BIN_DIRS_DEBUG )
set(openssl_OpenSSL_SSL_LIBRARY_TYPE_DEBUG STATIC)
set(openssl_OpenSSL_SSL_IS_HOST_WINDOWS_DEBUG 0)
set(openssl_OpenSSL_SSL_RES_DIRS_DEBUG )
set(openssl_OpenSSL_SSL_DEFINITIONS_DEBUG )
set(openssl_OpenSSL_SSL_OBJECTS_DEBUG )
set(openssl_OpenSSL_SSL_COMPILE_DEFINITIONS_DEBUG )
set(openssl_OpenSSL_SSL_COMPILE_OPTIONS_C_DEBUG "")
set(openssl_OpenSSL_SSL_COMPILE_OPTIONS_CXX_DEBUG "")
set(openssl_OpenSSL_SSL_LIBS_DEBUG ssl)
set(openssl_OpenSSL_SSL_SYSTEM_LIBS_DEBUG dl pthread)
set(openssl_OpenSSL_SSL_FRAMEWORK_DIRS_DEBUG )
set(openssl_OpenSSL_SSL_FRAMEWORKS_DEBUG )
set(openssl_OpenSSL_SSL_DEPENDENCIES_DEBUG OpenSSL::Crypto)
set(openssl_OpenSSL_SSL_SHARED_LINK_FLAGS_DEBUG )
set(openssl_OpenSSL_SSL_EXE_LINK_FLAGS_DEBUG )
set(openssl_OpenSSL_SSL_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(openssl_OpenSSL_SSL_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${openssl_OpenSSL_SSL_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${openssl_OpenSSL_SSL_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${openssl_OpenSSL_SSL_EXE_LINK_FLAGS_DEBUG}>
)
set(openssl_OpenSSL_SSL_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${openssl_OpenSSL_SSL_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${openssl_OpenSSL_SSL_COMPILE_OPTIONS_C_DEBUG}>")
########### COMPONENT OpenSSL::Crypto VARIABLES ############################################

set(openssl_OpenSSL_Crypto_INCLUDE_DIRS_DEBUG "${openssl_PACKAGE_FOLDER_DEBUG}/include")
set(openssl_OpenSSL_Crypto_LIB_DIRS_DEBUG "${openssl_PACKAGE_FOLDER_DEBUG}/lib")
set(openssl_OpenSSL_Crypto_BIN_DIRS_DEBUG )
set(openssl_OpenSSL_Crypto_LIBRARY_TYPE_DEBUG STATIC)
set(openssl_OpenSSL_Crypto_IS_HOST_WINDOWS_DEBUG 0)
set(openssl_OpenSSL_Crypto_RES_DIRS_DEBUG )
set(openssl_OpenSSL_Crypto_DEFINITIONS_DEBUG )
set(openssl_OpenSSL_Crypto_OBJECTS_DEBUG )
set(openssl_OpenSSL_Crypto_COMPILE_DEFINITIONS_DEBUG )
set(openssl_OpenSSL_Crypto_COMPILE_OPTIONS_C_DEBUG "")
set(openssl_OpenSSL_Crypto_COMPILE_OPTIONS_CXX_DEBUG "")
set(openssl_OpenSSL_Crypto_LIBS_DEBUG crypto)
set(openssl_OpenSSL_Crypto_SYSTEM_LIBS_DEBUG dl rt pthread)
set(openssl_OpenSSL_Crypto_FRAMEWORK_DIRS_DEBUG )
set(openssl_OpenSSL_Crypto_FRAMEWORKS_DEBUG )
set(openssl_OpenSSL_Crypto_DEPENDENCIES_DEBUG )
set(openssl_OpenSSL_Crypto_SHARED_LINK_FLAGS_DEBUG )
set(openssl_OpenSSL_Crypto_EXE_LINK_FLAGS_DEBUG )
set(openssl_OpenSSL_Crypto_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(openssl_OpenSSL_Crypto_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${openssl_OpenSSL_Crypto_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${openssl_OpenSSL_Crypto_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${openssl_OpenSSL_Crypto_EXE_LINK_FLAGS_DEBUG}>
)
set(openssl_OpenSSL_Crypto_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${openssl_OpenSSL_Crypto_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${openssl_OpenSSL_Crypto_COMPILE_OPTIONS_C_DEBUG}>")