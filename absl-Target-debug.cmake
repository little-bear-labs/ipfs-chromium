# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(abseil_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(abseil_FRAMEWORKS_FOUND_DEBUG "${abseil_FRAMEWORKS_DEBUG}" "${abseil_FRAMEWORK_DIRS_DEBUG}")

set(abseil_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET abseil_DEPS_TARGET)
    add_library(abseil_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET abseil_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${abseil_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${abseil_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:absl::config;absl::core_headers;absl::atomic_hook;absl::errno_saver;absl::log_severity;absl::base_internal;absl::base;absl::dynamic_annotations;absl::raw_logging_internal;absl::type_traits;absl::spinlock_wait;absl::algorithm;absl::meta;absl::utility;absl::cleanup_internal;absl::container_common;absl::compare;absl::compressed_tuple;absl::container_memory;absl::cord;absl::layout;absl::memory;absl::strings;absl::throw_delegate;absl::span;absl::inlined_vector_internal;absl::hash_function_defaults;absl::raw_hash_map;absl::algorithm_container;absl::raw_hash_set;absl::node_slot_policy;absl::hash;absl::exponential_biased;absl::sample_recorder;absl::synchronization;absl::hashtable_debug_hooks;absl::bits;absl::endian;absl::hash_policy_traits;absl::optional;absl::prefetch;absl::hashtablez_sampler;absl::debugging_internal;absl::demangle_internal;absl::malloc_internal;absl::stacktrace;absl::symbolize;absl::examine_stack;absl::leak_check;absl::flags_path_util;absl::flags_program_name;absl::str_format;absl::fast_type_id;absl::flags_commandlineflag_internal;absl::flags_commandlineflag;absl::flags_private_handle_accessor;absl::flags_config;absl::flat_hash_map;absl::flags_marshalling;absl::flags_internal;absl::flags_reflection;absl::flags;absl::flags_usage_internal;absl::flags_usage;absl::city;absl::fixed_array;absl::function_ref;absl::int128;absl::variant;absl::low_level_hash;absl::random_distributions;absl::random_internal_nonsecure_base;absl::random_internal_pcg_engine;absl::random_internal_pool_urbg;absl::random_internal_randen_engine;absl::random_seed_sequences;absl::random_internal_distribution_caller;absl::random_internal_fast_uniform_bits;absl::random_internal_generate_real;absl::random_internal_fastmath;absl::random_internal_iostream_state_saver;absl::random_internal_traits;absl::random_internal_uniform_helper;absl::random_internal_wide_multiply;absl::inlined_vector;absl::random_internal_salted_seed_seq;absl::random_internal_seed_material;absl::random_seed_gen_exception;absl::random_internal_randen;absl::random_internal_platform;absl::random_internal_randen_hwaes;absl::random_internal_randen_slow;absl::random_internal_randen_hwaes_impl;absl::strerror;absl::status;absl::strings_internal;absl::str_format_internal;absl::numeric_representation;absl::cordz_update_tracker;absl::cord_internal;absl::cordz_functions;absl::cordz_handle;absl::cordz_statistics;absl::cordz_info;absl::cordz_update_scope;absl::time;absl::graphcycles_internal;absl::kernel_timeout_internal;absl::civil_time;absl::time_zone;absl::bad_any_cast;absl::bad_any_cast_impl;absl::bad_optional_access;absl::bad_variant_access>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### abseil_DEPS_TARGET to all of them
conan_package_library_targets("${abseil_LIBS_DEBUG}"    # libraries
                              "${abseil_LIB_DIRS_DEBUG}" # package_libdir
                              "${abseil_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_LIBRARY_TYPE_DEBUG}"
                              "${abseil_IS_HOST_WINDOWS_DEBUG}"
                              abseil_DEPS_TARGET
                              abseil_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "abseil"    # package_name
                              "${abseil_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${abseil_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Debug ########################################

    ########## COMPONENT absl::flags_parse #############

        set(abseil_absl_flags_parse_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_parse_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_parse_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_parse_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_parse_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_parse_DEPS_TARGET)
            add_library(abseil_absl_flags_parse_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_parse_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_parse_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_parse_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_parse_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_parse_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_parse_LIBS_DEBUG}"
                              "${abseil_absl_flags_parse_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_parse_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_parse_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_parse_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_parse_DEPS_TARGET
                              abseil_absl_flags_parse_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_parse"
                              "${abseil_absl_flags_parse_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_parse
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_parse_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_parse_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_parse_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_parse
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_parse_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_parse PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_parse_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_parse PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_parse_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_parse PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_parse_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_parse PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_parse_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_parse PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_parse_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_usage #############

        set(abseil_absl_flags_usage_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_usage_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_usage_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_usage_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_usage_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_usage_DEPS_TARGET)
            add_library(abseil_absl_flags_usage_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_usage_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_usage_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_usage_LIBS_DEBUG}"
                              "${abseil_absl_flags_usage_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_usage_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_usage_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_usage_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_usage_DEPS_TARGET
                              abseil_absl_flags_usage_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_usage"
                              "${abseil_absl_flags_usage_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_usage
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_usage_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_usage
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_usage_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_usage PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_usage PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_usage PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_usage PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_usage PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_usage_internal #############

        set(abseil_absl_flags_usage_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_usage_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_usage_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_usage_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_usage_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_usage_internal_DEPS_TARGET)
            add_library(abseil_absl_flags_usage_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_usage_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_usage_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_usage_internal_LIBS_DEBUG}"
                              "${abseil_absl_flags_usage_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_usage_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_usage_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_usage_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_usage_internal_DEPS_TARGET
                              abseil_absl_flags_usage_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_usage_internal"
                              "${abseil_absl_flags_usage_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_usage_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_usage_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_usage_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_usage_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_usage_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_usage_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_usage_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_usage_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_usage_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_usage_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags #############

        set(abseil_absl_flags_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_DEPS_TARGET)
            add_library(abseil_absl_flags_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_LIBS_DEBUG}"
                              "${abseil_absl_flags_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_DEPS_TARGET
                              abseil_absl_flags_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags"
                              "${abseil_absl_flags_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_reflection #############

        set(abseil_absl_flags_reflection_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_reflection_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_reflection_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_reflection_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_reflection_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_reflection_DEPS_TARGET)
            add_library(abseil_absl_flags_reflection_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_reflection_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_reflection_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_reflection_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_reflection_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_reflection_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_reflection_LIBS_DEBUG}"
                              "${abseil_absl_flags_reflection_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_reflection_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_reflection_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_reflection_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_reflection_DEPS_TARGET
                              abseil_absl_flags_reflection_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_reflection"
                              "${abseil_absl_flags_reflection_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_reflection
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_reflection_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_reflection_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_reflection_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_reflection
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_reflection_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_reflection PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_reflection_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_reflection PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_reflection_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_reflection PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_reflection_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_reflection PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_reflection_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_reflection PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_reflection_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::node_hash_map #############

        set(abseil_absl_node_hash_map_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_node_hash_map_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_node_hash_map_FRAMEWORKS_DEBUG}" "${abseil_absl_node_hash_map_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_node_hash_map_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_node_hash_map_DEPS_TARGET)
            add_library(abseil_absl_node_hash_map_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_node_hash_map_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_map_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_map_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_map_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_node_hash_map_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_node_hash_map_LIBS_DEBUG}"
                              "${abseil_absl_node_hash_map_LIB_DIRS_DEBUG}"
                              "${abseil_absl_node_hash_map_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_node_hash_map_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_node_hash_map_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_node_hash_map_DEPS_TARGET
                              abseil_absl_node_hash_map_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_node_hash_map"
                              "${abseil_absl_node_hash_map_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::node_hash_map
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_map_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_map_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_node_hash_map_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::node_hash_map
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_node_hash_map_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::node_hash_map PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_map_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::node_hash_map PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_map_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::node_hash_map PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_map_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::node_hash_map PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_map_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::node_hash_map PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_map_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flat_hash_map #############

        set(abseil_absl_flat_hash_map_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flat_hash_map_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flat_hash_map_FRAMEWORKS_DEBUG}" "${abseil_absl_flat_hash_map_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flat_hash_map_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flat_hash_map_DEPS_TARGET)
            add_library(abseil_absl_flat_hash_map_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flat_hash_map_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_map_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_map_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_map_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flat_hash_map_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flat_hash_map_LIBS_DEBUG}"
                              "${abseil_absl_flat_hash_map_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flat_hash_map_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flat_hash_map_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flat_hash_map_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flat_hash_map_DEPS_TARGET
                              abseil_absl_flat_hash_map_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flat_hash_map"
                              "${abseil_absl_flat_hash_map_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flat_hash_map
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_map_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_map_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flat_hash_map_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flat_hash_map
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flat_hash_map_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flat_hash_map PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_map_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flat_hash_map PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_map_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flat_hash_map PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_map_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flat_hash_map PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_map_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flat_hash_map PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_map_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::raw_hash_map #############

        set(abseil_absl_raw_hash_map_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_raw_hash_map_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_raw_hash_map_FRAMEWORKS_DEBUG}" "${abseil_absl_raw_hash_map_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_raw_hash_map_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_raw_hash_map_DEPS_TARGET)
            add_library(abseil_absl_raw_hash_map_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_raw_hash_map_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_map_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_map_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_map_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_raw_hash_map_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_raw_hash_map_LIBS_DEBUG}"
                              "${abseil_absl_raw_hash_map_LIB_DIRS_DEBUG}"
                              "${abseil_absl_raw_hash_map_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_raw_hash_map_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_raw_hash_map_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_raw_hash_map_DEPS_TARGET
                              abseil_absl_raw_hash_map_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_raw_hash_map"
                              "${abseil_absl_raw_hash_map_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::raw_hash_map
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_map_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_map_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_raw_hash_map_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::raw_hash_map
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_raw_hash_map_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::raw_hash_map PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_map_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_hash_map PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_map_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_hash_map PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_map_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_hash_map PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_map_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_hash_map PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_map_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::node_hash_set #############

        set(abseil_absl_node_hash_set_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_node_hash_set_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_node_hash_set_FRAMEWORKS_DEBUG}" "${abseil_absl_node_hash_set_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_node_hash_set_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_node_hash_set_DEPS_TARGET)
            add_library(abseil_absl_node_hash_set_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_node_hash_set_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_set_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_set_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_set_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_node_hash_set_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_node_hash_set_LIBS_DEBUG}"
                              "${abseil_absl_node_hash_set_LIB_DIRS_DEBUG}"
                              "${abseil_absl_node_hash_set_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_node_hash_set_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_node_hash_set_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_node_hash_set_DEPS_TARGET
                              abseil_absl_node_hash_set_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_node_hash_set"
                              "${abseil_absl_node_hash_set_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::node_hash_set
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_set_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_set_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_node_hash_set_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::node_hash_set
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_node_hash_set_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::node_hash_set PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_set_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::node_hash_set PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_set_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::node_hash_set PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_set_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::node_hash_set PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_set_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::node_hash_set PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_node_hash_set_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flat_hash_set #############

        set(abseil_absl_flat_hash_set_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flat_hash_set_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flat_hash_set_FRAMEWORKS_DEBUG}" "${abseil_absl_flat_hash_set_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flat_hash_set_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flat_hash_set_DEPS_TARGET)
            add_library(abseil_absl_flat_hash_set_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flat_hash_set_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_set_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_set_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_set_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flat_hash_set_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flat_hash_set_LIBS_DEBUG}"
                              "${abseil_absl_flat_hash_set_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flat_hash_set_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flat_hash_set_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flat_hash_set_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flat_hash_set_DEPS_TARGET
                              abseil_absl_flat_hash_set_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flat_hash_set"
                              "${abseil_absl_flat_hash_set_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flat_hash_set
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_set_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_set_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flat_hash_set_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flat_hash_set
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flat_hash_set_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flat_hash_set PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_set_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flat_hash_set PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_set_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flat_hash_set PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_set_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flat_hash_set PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_set_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flat_hash_set PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flat_hash_set_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::statusor #############

        set(abseil_absl_statusor_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_statusor_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_statusor_FRAMEWORKS_DEBUG}" "${abseil_absl_statusor_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_statusor_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_statusor_DEPS_TARGET)
            add_library(abseil_absl_statusor_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_statusor_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_statusor_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_statusor_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_statusor_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_statusor_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_statusor_LIBS_DEBUG}"
                              "${abseil_absl_statusor_LIB_DIRS_DEBUG}"
                              "${abseil_absl_statusor_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_statusor_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_statusor_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_statusor_DEPS_TARGET
                              abseil_absl_statusor_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_statusor"
                              "${abseil_absl_statusor_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::statusor
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_statusor_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_statusor_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_statusor_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::statusor
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_statusor_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::statusor PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_statusor_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::statusor PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_statusor_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::statusor PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_statusor_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::statusor PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_statusor_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::statusor PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_statusor_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::status #############

        set(abseil_absl_status_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_status_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_status_FRAMEWORKS_DEBUG}" "${abseil_absl_status_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_status_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_status_DEPS_TARGET)
            add_library(abseil_absl_status_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_status_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_status_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_status_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_status_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_status_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_status_LIBS_DEBUG}"
                              "${abseil_absl_status_LIB_DIRS_DEBUG}"
                              "${abseil_absl_status_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_status_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_status_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_status_DEPS_TARGET
                              abseil_absl_status_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_status"
                              "${abseil_absl_status_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::status
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_status_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_status_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_status_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::status
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_status_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::status PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_status_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::status PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_status_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::status PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_status_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::status PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_status_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::status PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_status_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_random #############

        set(abseil_absl_random_random_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_random_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_random_FRAMEWORKS_DEBUG}" "${abseil_absl_random_random_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_random_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_random_DEPS_TARGET)
            add_library(abseil_absl_random_random_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_random_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_random_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_random_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_random_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_random_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_random_LIBS_DEBUG}"
                              "${abseil_absl_random_random_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_random_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_random_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_random_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_random_DEPS_TARGET
                              abseil_absl_random_random_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_random"
                              "${abseil_absl_random_random_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_random
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_random_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_random_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_random_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_random
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_random_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_random PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_random_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_random PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_random_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_random PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_random_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_random PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_random_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_random PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_random_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::raw_hash_set #############

        set(abseil_absl_raw_hash_set_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_raw_hash_set_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_raw_hash_set_FRAMEWORKS_DEBUG}" "${abseil_absl_raw_hash_set_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_raw_hash_set_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_raw_hash_set_DEPS_TARGET)
            add_library(abseil_absl_raw_hash_set_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_raw_hash_set_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_set_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_set_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_set_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_raw_hash_set_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_raw_hash_set_LIBS_DEBUG}"
                              "${abseil_absl_raw_hash_set_LIB_DIRS_DEBUG}"
                              "${abseil_absl_raw_hash_set_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_raw_hash_set_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_raw_hash_set_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_raw_hash_set_DEPS_TARGET
                              abseil_absl_raw_hash_set_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_raw_hash_set"
                              "${abseil_absl_raw_hash_set_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::raw_hash_set
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_set_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_set_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_raw_hash_set_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::raw_hash_set
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_raw_hash_set_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::raw_hash_set PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_set_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_hash_set PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_set_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_hash_set PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_set_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_hash_set PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_set_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_hash_set PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_raw_hash_set_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::hashtablez_sampler #############

        set(abseil_absl_hashtablez_sampler_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_hashtablez_sampler_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_hashtablez_sampler_FRAMEWORKS_DEBUG}" "${abseil_absl_hashtablez_sampler_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_hashtablez_sampler_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_hashtablez_sampler_DEPS_TARGET)
            add_library(abseil_absl_hashtablez_sampler_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_hashtablez_sampler_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtablez_sampler_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hashtablez_sampler_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hashtablez_sampler_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_hashtablez_sampler_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_hashtablez_sampler_LIBS_DEBUG}"
                              "${abseil_absl_hashtablez_sampler_LIB_DIRS_DEBUG}"
                              "${abseil_absl_hashtablez_sampler_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_hashtablez_sampler_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_hashtablez_sampler_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_hashtablez_sampler_DEPS_TARGET
                              abseil_absl_hashtablez_sampler_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_hashtablez_sampler"
                              "${abseil_absl_hashtablez_sampler_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::hashtablez_sampler
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtablez_sampler_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hashtablez_sampler_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_hashtablez_sampler_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::hashtablez_sampler
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_hashtablez_sampler_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::hashtablez_sampler PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hashtablez_sampler_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtablez_sampler PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtablez_sampler_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtablez_sampler PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtablez_sampler_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtablez_sampler PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hashtablez_sampler_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtablez_sampler PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hashtablez_sampler_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::hash_function_defaults #############

        set(abseil_absl_hash_function_defaults_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_hash_function_defaults_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_hash_function_defaults_FRAMEWORKS_DEBUG}" "${abseil_absl_hash_function_defaults_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_hash_function_defaults_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_hash_function_defaults_DEPS_TARGET)
            add_library(abseil_absl_hash_function_defaults_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_hash_function_defaults_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_function_defaults_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hash_function_defaults_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hash_function_defaults_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_hash_function_defaults_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_hash_function_defaults_LIBS_DEBUG}"
                              "${abseil_absl_hash_function_defaults_LIB_DIRS_DEBUG}"
                              "${abseil_absl_hash_function_defaults_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_hash_function_defaults_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_hash_function_defaults_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_hash_function_defaults_DEPS_TARGET
                              abseil_absl_hash_function_defaults_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_hash_function_defaults"
                              "${abseil_absl_hash_function_defaults_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::hash_function_defaults
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_function_defaults_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hash_function_defaults_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_hash_function_defaults_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::hash_function_defaults
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_hash_function_defaults_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::hash_function_defaults PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hash_function_defaults_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::hash_function_defaults PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_function_defaults_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hash_function_defaults PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_function_defaults_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hash_function_defaults PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hash_function_defaults_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::hash_function_defaults PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hash_function_defaults_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::btree #############

        set(abseil_absl_btree_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_btree_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_btree_FRAMEWORKS_DEBUG}" "${abseil_absl_btree_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_btree_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_btree_DEPS_TARGET)
            add_library(abseil_absl_btree_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_btree_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_btree_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_btree_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_btree_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_btree_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_btree_LIBS_DEBUG}"
                              "${abseil_absl_btree_LIB_DIRS_DEBUG}"
                              "${abseil_absl_btree_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_btree_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_btree_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_btree_DEPS_TARGET
                              abseil_absl_btree_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_btree"
                              "${abseil_absl_btree_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::btree
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_btree_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_btree_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_btree_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::btree
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_btree_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::btree PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_btree_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::btree PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_btree_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::btree PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_btree_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::btree PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_btree_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::btree PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_btree_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cord #############

        set(abseil_absl_cord_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cord_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cord_FRAMEWORKS_DEBUG}" "${abseil_absl_cord_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cord_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cord_DEPS_TARGET)
            add_library(abseil_absl_cord_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cord_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cord_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cord_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cord_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cord_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cord_LIBS_DEBUG}"
                              "${abseil_absl_cord_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cord_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cord_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cord_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cord_DEPS_TARGET
                              abseil_absl_cord_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cord"
                              "${abseil_absl_cord_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cord
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cord_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cord_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cord_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cord
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cord_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cord PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cord_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cord PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cord_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cord PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cord_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cord PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cord_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cord PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cord_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cordz_update_scope #############

        set(abseil_absl_cordz_update_scope_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cordz_update_scope_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cordz_update_scope_FRAMEWORKS_DEBUG}" "${abseil_absl_cordz_update_scope_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cordz_update_scope_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cordz_update_scope_DEPS_TARGET)
            add_library(abseil_absl_cordz_update_scope_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cordz_update_scope_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_scope_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_scope_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_scope_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cordz_update_scope_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cordz_update_scope_LIBS_DEBUG}"
                              "${abseil_absl_cordz_update_scope_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cordz_update_scope_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cordz_update_scope_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cordz_update_scope_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cordz_update_scope_DEPS_TARGET
                              abseil_absl_cordz_update_scope_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cordz_update_scope"
                              "${abseil_absl_cordz_update_scope_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cordz_update_scope
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_scope_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_scope_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cordz_update_scope_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cordz_update_scope
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cordz_update_scope_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cordz_update_scope PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_scope_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_update_scope PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_scope_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_update_scope PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_scope_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_update_scope PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_scope_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_update_scope PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_scope_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cordz_sample_token #############

        set(abseil_absl_cordz_sample_token_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cordz_sample_token_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cordz_sample_token_FRAMEWORKS_DEBUG}" "${abseil_absl_cordz_sample_token_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cordz_sample_token_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cordz_sample_token_DEPS_TARGET)
            add_library(abseil_absl_cordz_sample_token_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cordz_sample_token_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_sample_token_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_sample_token_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_sample_token_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cordz_sample_token_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cordz_sample_token_LIBS_DEBUG}"
                              "${abseil_absl_cordz_sample_token_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cordz_sample_token_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cordz_sample_token_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cordz_sample_token_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cordz_sample_token_DEPS_TARGET
                              abseil_absl_cordz_sample_token_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cordz_sample_token"
                              "${abseil_absl_cordz_sample_token_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cordz_sample_token
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_sample_token_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_sample_token_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cordz_sample_token_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cordz_sample_token
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cordz_sample_token_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cordz_sample_token PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_sample_token_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_sample_token PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_sample_token_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_sample_token PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_sample_token_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_sample_token PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_sample_token_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_sample_token PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_sample_token_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cordz_info #############

        set(abseil_absl_cordz_info_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cordz_info_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cordz_info_FRAMEWORKS_DEBUG}" "${abseil_absl_cordz_info_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cordz_info_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cordz_info_DEPS_TARGET)
            add_library(abseil_absl_cordz_info_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cordz_info_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_info_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_info_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_info_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cordz_info_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cordz_info_LIBS_DEBUG}"
                              "${abseil_absl_cordz_info_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cordz_info_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cordz_info_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cordz_info_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cordz_info_DEPS_TARGET
                              abseil_absl_cordz_info_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cordz_info"
                              "${abseil_absl_cordz_info_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cordz_info
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_info_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_info_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cordz_info_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cordz_info
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cordz_info_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cordz_info PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_info_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_info PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_info_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_info PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_info_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_info PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_info_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_info PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_info_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cordz_handle #############

        set(abseil_absl_cordz_handle_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cordz_handle_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cordz_handle_FRAMEWORKS_DEBUG}" "${abseil_absl_cordz_handle_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cordz_handle_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cordz_handle_DEPS_TARGET)
            add_library(abseil_absl_cordz_handle_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cordz_handle_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_handle_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_handle_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_handle_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cordz_handle_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cordz_handle_LIBS_DEBUG}"
                              "${abseil_absl_cordz_handle_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cordz_handle_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cordz_handle_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cordz_handle_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cordz_handle_DEPS_TARGET
                              abseil_absl_cordz_handle_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cordz_handle"
                              "${abseil_absl_cordz_handle_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cordz_handle
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_handle_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_handle_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cordz_handle_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cordz_handle
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cordz_handle_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cordz_handle PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_handle_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_handle PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_handle_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_handle PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_handle_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_handle PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_handle_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_handle PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_handle_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cordz_statistics #############

        set(abseil_absl_cordz_statistics_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cordz_statistics_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cordz_statistics_FRAMEWORKS_DEBUG}" "${abseil_absl_cordz_statistics_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cordz_statistics_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cordz_statistics_DEPS_TARGET)
            add_library(abseil_absl_cordz_statistics_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cordz_statistics_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_statistics_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_statistics_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_statistics_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cordz_statistics_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cordz_statistics_LIBS_DEBUG}"
                              "${abseil_absl_cordz_statistics_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cordz_statistics_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cordz_statistics_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cordz_statistics_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cordz_statistics_DEPS_TARGET
                              abseil_absl_cordz_statistics_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cordz_statistics"
                              "${abseil_absl_cordz_statistics_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cordz_statistics
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_statistics_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_statistics_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cordz_statistics_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cordz_statistics
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cordz_statistics_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cordz_statistics PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_statistics_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_statistics PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_statistics_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_statistics PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_statistics_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_statistics PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_statistics_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_statistics PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_statistics_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_distribution_test_util #############

        set(abseil_absl_random_internal_distribution_test_util_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_distribution_test_util_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_distribution_test_util_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_distribution_test_util_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_distribution_test_util_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_distribution_test_util_DEPS_TARGET)
            add_library(abseil_absl_random_internal_distribution_test_util_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_distribution_test_util_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_test_util_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_test_util_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_test_util_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_distribution_test_util_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_distribution_test_util_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_distribution_test_util_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_distribution_test_util_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_distribution_test_util_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_distribution_test_util_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_distribution_test_util_DEPS_TARGET
                              abseil_absl_random_internal_distribution_test_util_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_distribution_test_util"
                              "${abseil_absl_random_internal_distribution_test_util_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_distribution_test_util
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_test_util_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_test_util_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_distribution_test_util_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_distribution_test_util
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_distribution_test_util_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_distribution_test_util PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_test_util_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_distribution_test_util PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_test_util_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_distribution_test_util PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_test_util_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_distribution_test_util PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_test_util_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_distribution_test_util PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_test_util_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_seed_sequences #############

        set(abseil_absl_random_seed_sequences_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_seed_sequences_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_seed_sequences_FRAMEWORKS_DEBUG}" "${abseil_absl_random_seed_sequences_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_seed_sequences_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_seed_sequences_DEPS_TARGET)
            add_library(abseil_absl_random_seed_sequences_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_seed_sequences_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_sequences_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_sequences_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_sequences_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_seed_sequences_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_seed_sequences_LIBS_DEBUG}"
                              "${abseil_absl_random_seed_sequences_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_seed_sequences_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_seed_sequences_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_seed_sequences_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_seed_sequences_DEPS_TARGET
                              abseil_absl_random_seed_sequences_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_seed_sequences"
                              "${abseil_absl_random_seed_sequences_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_seed_sequences
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_sequences_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_sequences_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_seed_sequences_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_seed_sequences
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_seed_sequences_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_seed_sequences PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_sequences_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_seed_sequences PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_sequences_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_seed_sequences PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_sequences_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_seed_sequences PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_sequences_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_seed_sequences PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_sequences_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::sample_recorder #############

        set(abseil_absl_sample_recorder_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_sample_recorder_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_sample_recorder_FRAMEWORKS_DEBUG}" "${abseil_absl_sample_recorder_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_sample_recorder_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_sample_recorder_DEPS_TARGET)
            add_library(abseil_absl_sample_recorder_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_sample_recorder_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_sample_recorder_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_sample_recorder_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_sample_recorder_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_sample_recorder_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_sample_recorder_LIBS_DEBUG}"
                              "${abseil_absl_sample_recorder_LIB_DIRS_DEBUG}"
                              "${abseil_absl_sample_recorder_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_sample_recorder_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_sample_recorder_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_sample_recorder_DEPS_TARGET
                              abseil_absl_sample_recorder_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_sample_recorder"
                              "${abseil_absl_sample_recorder_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::sample_recorder
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_sample_recorder_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_sample_recorder_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_sample_recorder_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::sample_recorder
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_sample_recorder_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::sample_recorder PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_sample_recorder_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::sample_recorder PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_sample_recorder_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::sample_recorder PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_sample_recorder_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::sample_recorder PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_sample_recorder_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::sample_recorder PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_sample_recorder_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_internal #############

        set(abseil_absl_flags_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_internal_DEPS_TARGET)
            add_library(abseil_absl_flags_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_internal_LIBS_DEBUG}"
                              "${abseil_absl_flags_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_internal_DEPS_TARGET
                              abseil_absl_flags_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_internal"
                              "${abseil_absl_flags_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_marshalling #############

        set(abseil_absl_flags_marshalling_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_marshalling_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_marshalling_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_marshalling_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_marshalling_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_marshalling_DEPS_TARGET)
            add_library(abseil_absl_flags_marshalling_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_marshalling_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_marshalling_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_marshalling_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_marshalling_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_marshalling_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_marshalling_LIBS_DEBUG}"
                              "${abseil_absl_flags_marshalling_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_marshalling_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_marshalling_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_marshalling_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_marshalling_DEPS_TARGET
                              abseil_absl_flags_marshalling_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_marshalling"
                              "${abseil_absl_flags_marshalling_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_marshalling
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_marshalling_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_marshalling_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_marshalling_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_marshalling
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_marshalling_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_marshalling PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_marshalling_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_marshalling PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_marshalling_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_marshalling PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_marshalling_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_marshalling PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_marshalling_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_marshalling PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_marshalling_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_config #############

        set(abseil_absl_flags_config_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_config_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_config_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_config_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_config_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_config_DEPS_TARGET)
            add_library(abseil_absl_flags_config_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_config_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_config_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_config_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_config_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_config_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_config_LIBS_DEBUG}"
                              "${abseil_absl_flags_config_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_config_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_config_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_config_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_config_DEPS_TARGET
                              abseil_absl_flags_config_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_config"
                              "${abseil_absl_flags_config_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_config
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_config_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_config_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_config_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_config
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_config_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_config PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_config_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_config PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_config_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_config PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_config_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_config PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_config_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_config PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_config_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_program_name #############

        set(abseil_absl_flags_program_name_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_program_name_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_program_name_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_program_name_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_program_name_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_program_name_DEPS_TARGET)
            add_library(abseil_absl_flags_program_name_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_program_name_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_program_name_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_program_name_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_program_name_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_program_name_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_program_name_LIBS_DEBUG}"
                              "${abseil_absl_flags_program_name_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_program_name_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_program_name_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_program_name_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_program_name_DEPS_TARGET
                              abseil_absl_flags_program_name_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_program_name"
                              "${abseil_absl_flags_program_name_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_program_name
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_program_name_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_program_name_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_program_name_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_program_name
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_program_name_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_program_name PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_program_name_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_program_name PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_program_name_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_program_name PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_program_name_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_program_name PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_program_name_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_program_name PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_program_name_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::synchronization #############

        set(abseil_absl_synchronization_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_synchronization_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_synchronization_FRAMEWORKS_DEBUG}" "${abseil_absl_synchronization_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_synchronization_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_synchronization_DEPS_TARGET)
            add_library(abseil_absl_synchronization_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_synchronization_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_synchronization_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_synchronization_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_synchronization_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_synchronization_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_synchronization_LIBS_DEBUG}"
                              "${abseil_absl_synchronization_LIB_DIRS_DEBUG}"
                              "${abseil_absl_synchronization_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_synchronization_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_synchronization_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_synchronization_DEPS_TARGET
                              abseil_absl_synchronization_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_synchronization"
                              "${abseil_absl_synchronization_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::synchronization
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_synchronization_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_synchronization_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_synchronization_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::synchronization
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_synchronization_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::synchronization PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_synchronization_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::synchronization PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_synchronization_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::synchronization PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_synchronization_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::synchronization PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_synchronization_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::synchronization PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_synchronization_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::kernel_timeout_internal #############

        set(abseil_absl_kernel_timeout_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_kernel_timeout_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_kernel_timeout_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_kernel_timeout_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_kernel_timeout_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_kernel_timeout_internal_DEPS_TARGET)
            add_library(abseil_absl_kernel_timeout_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_kernel_timeout_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_kernel_timeout_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_kernel_timeout_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_kernel_timeout_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_kernel_timeout_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_kernel_timeout_internal_LIBS_DEBUG}"
                              "${abseil_absl_kernel_timeout_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_kernel_timeout_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_kernel_timeout_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_kernel_timeout_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_kernel_timeout_internal_DEPS_TARGET
                              abseil_absl_kernel_timeout_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_kernel_timeout_internal"
                              "${abseil_absl_kernel_timeout_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::kernel_timeout_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_kernel_timeout_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_kernel_timeout_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_kernel_timeout_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::kernel_timeout_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_kernel_timeout_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::kernel_timeout_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_kernel_timeout_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::kernel_timeout_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_kernel_timeout_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::kernel_timeout_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_kernel_timeout_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::kernel_timeout_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_kernel_timeout_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::kernel_timeout_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_kernel_timeout_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cord_internal #############

        set(abseil_absl_cord_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cord_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cord_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_cord_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cord_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cord_internal_DEPS_TARGET)
            add_library(abseil_absl_cord_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cord_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cord_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cord_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cord_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cord_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cord_internal_LIBS_DEBUG}"
                              "${abseil_absl_cord_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cord_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cord_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cord_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cord_internal_DEPS_TARGET
                              abseil_absl_cord_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cord_internal"
                              "${abseil_absl_cord_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cord_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cord_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cord_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cord_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cord_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cord_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cord_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cord_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cord_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cord_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cord_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cord_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cord_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cord_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cord_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cord_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::str_format #############

        set(abseil_absl_str_format_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_str_format_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_str_format_FRAMEWORKS_DEBUG}" "${abseil_absl_str_format_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_str_format_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_str_format_DEPS_TARGET)
            add_library(abseil_absl_str_format_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_str_format_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_str_format_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_str_format_LIBS_DEBUG}"
                              "${abseil_absl_str_format_LIB_DIRS_DEBUG}"
                              "${abseil_absl_str_format_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_str_format_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_str_format_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_str_format_DEPS_TARGET
                              abseil_absl_str_format_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_str_format"
                              "${abseil_absl_str_format_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::str_format
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_str_format_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::str_format
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_str_format_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::str_format PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::str_format PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::str_format PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::str_format PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::str_format PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_nonsecure_base #############

        set(abseil_absl_random_internal_nonsecure_base_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_nonsecure_base_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_nonsecure_base_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_nonsecure_base_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_nonsecure_base_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_nonsecure_base_DEPS_TARGET)
            add_library(abseil_absl_random_internal_nonsecure_base_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_nonsecure_base_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_nonsecure_base_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_nonsecure_base_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_nonsecure_base_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_nonsecure_base_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_nonsecure_base_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_nonsecure_base_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_nonsecure_base_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_nonsecure_base_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_nonsecure_base_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_nonsecure_base_DEPS_TARGET
                              abseil_absl_random_internal_nonsecure_base_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_nonsecure_base"
                              "${abseil_absl_random_internal_nonsecure_base_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_nonsecure_base
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_nonsecure_base_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_nonsecure_base_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_nonsecure_base_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_nonsecure_base
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_nonsecure_base_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_nonsecure_base PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_nonsecure_base_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_nonsecure_base PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_nonsecure_base_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_nonsecure_base PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_nonsecure_base_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_nonsecure_base PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_nonsecure_base_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_nonsecure_base PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_nonsecure_base_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_salted_seed_seq #############

        set(abseil_absl_random_internal_salted_seed_seq_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_salted_seed_seq_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_salted_seed_seq_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_salted_seed_seq_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_salted_seed_seq_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_salted_seed_seq_DEPS_TARGET)
            add_library(abseil_absl_random_internal_salted_seed_seq_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_salted_seed_seq_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_salted_seed_seq_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_salted_seed_seq_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_salted_seed_seq_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_salted_seed_seq_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_salted_seed_seq_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_salted_seed_seq_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_salted_seed_seq_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_salted_seed_seq_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_salted_seed_seq_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_salted_seed_seq_DEPS_TARGET
                              abseil_absl_random_internal_salted_seed_seq_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_salted_seed_seq"
                              "${abseil_absl_random_internal_salted_seed_seq_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_salted_seed_seq
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_salted_seed_seq_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_salted_seed_seq_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_salted_seed_seq_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_salted_seed_seq
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_salted_seed_seq_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_salted_seed_seq PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_salted_seed_seq_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_salted_seed_seq PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_salted_seed_seq_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_salted_seed_seq PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_salted_seed_seq_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_salted_seed_seq PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_salted_seed_seq_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_salted_seed_seq PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_salted_seed_seq_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_pool_urbg #############

        set(abseil_absl_random_internal_pool_urbg_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_pool_urbg_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_pool_urbg_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_pool_urbg_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_pool_urbg_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_pool_urbg_DEPS_TARGET)
            add_library(abseil_absl_random_internal_pool_urbg_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_pool_urbg_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pool_urbg_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pool_urbg_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pool_urbg_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_pool_urbg_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_pool_urbg_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_pool_urbg_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_pool_urbg_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_pool_urbg_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_pool_urbg_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_pool_urbg_DEPS_TARGET
                              abseil_absl_random_internal_pool_urbg_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_pool_urbg"
                              "${abseil_absl_random_internal_pool_urbg_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_pool_urbg
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pool_urbg_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pool_urbg_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_pool_urbg_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_pool_urbg
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_pool_urbg_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_pool_urbg PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pool_urbg_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_pool_urbg PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pool_urbg_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_pool_urbg PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pool_urbg_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_pool_urbg PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pool_urbg_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_pool_urbg PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pool_urbg_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_seed_material #############

        set(abseil_absl_random_internal_seed_material_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_seed_material_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_seed_material_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_seed_material_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_seed_material_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_seed_material_DEPS_TARGET)
            add_library(abseil_absl_random_internal_seed_material_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_seed_material_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_seed_material_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_seed_material_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_seed_material_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_seed_material_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_seed_material_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_seed_material_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_seed_material_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_seed_material_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_seed_material_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_seed_material_DEPS_TARGET
                              abseil_absl_random_internal_seed_material_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_seed_material"
                              "${abseil_absl_random_internal_seed_material_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_seed_material
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_seed_material_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_seed_material_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_seed_material_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_seed_material
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_seed_material_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_seed_material PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_seed_material_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_seed_material PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_seed_material_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_seed_material PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_seed_material_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_seed_material PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_seed_material_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_seed_material PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_seed_material_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_distributions #############

        set(abseil_absl_random_distributions_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_distributions_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_distributions_FRAMEWORKS_DEBUG}" "${abseil_absl_random_distributions_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_distributions_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_distributions_DEPS_TARGET)
            add_library(abseil_absl_random_distributions_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_distributions_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_distributions_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_distributions_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_distributions_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_distributions_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_distributions_LIBS_DEBUG}"
                              "${abseil_absl_random_distributions_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_distributions_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_distributions_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_distributions_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_distributions_DEPS_TARGET
                              abseil_absl_random_distributions_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_distributions"
                              "${abseil_absl_random_distributions_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_distributions
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_distributions_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_distributions_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_distributions_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_distributions
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_distributions_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_distributions PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_distributions_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_distributions PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_distributions_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_distributions PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_distributions_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_distributions PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_distributions_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_distributions PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_distributions_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::hash #############

        set(abseil_absl_hash_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_hash_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_hash_FRAMEWORKS_DEBUG}" "${abseil_absl_hash_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_hash_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_hash_DEPS_TARGET)
            add_library(abseil_absl_hash_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_hash_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hash_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hash_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_hash_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_hash_LIBS_DEBUG}"
                              "${abseil_absl_hash_LIB_DIRS_DEBUG}"
                              "${abseil_absl_hash_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_hash_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_hash_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_hash_DEPS_TARGET
                              abseil_absl_hash_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_hash"
                              "${abseil_absl_hash_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::hash
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hash_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_hash_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::hash
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_hash_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::hash PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hash_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::hash PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hash PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hash PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hash_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::hash PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hash_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_private_handle_accessor #############

        set(abseil_absl_flags_private_handle_accessor_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_private_handle_accessor_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_private_handle_accessor_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_private_handle_accessor_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_private_handle_accessor_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_private_handle_accessor_DEPS_TARGET)
            add_library(abseil_absl_flags_private_handle_accessor_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_private_handle_accessor_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_private_handle_accessor_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_private_handle_accessor_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_private_handle_accessor_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_private_handle_accessor_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_private_handle_accessor_LIBS_DEBUG}"
                              "${abseil_absl_flags_private_handle_accessor_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_private_handle_accessor_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_private_handle_accessor_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_private_handle_accessor_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_private_handle_accessor_DEPS_TARGET
                              abseil_absl_flags_private_handle_accessor_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_private_handle_accessor"
                              "${abseil_absl_flags_private_handle_accessor_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_private_handle_accessor
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_private_handle_accessor_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_private_handle_accessor_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_private_handle_accessor_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_private_handle_accessor
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_private_handle_accessor_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_private_handle_accessor PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_private_handle_accessor_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_private_handle_accessor PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_private_handle_accessor_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_private_handle_accessor PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_private_handle_accessor_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_private_handle_accessor PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_private_handle_accessor_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_private_handle_accessor PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_private_handle_accessor_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_commandlineflag #############

        set(abseil_absl_flags_commandlineflag_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_commandlineflag_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_commandlineflag_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_commandlineflag_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_commandlineflag_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_commandlineflag_DEPS_TARGET)
            add_library(abseil_absl_flags_commandlineflag_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_commandlineflag_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_commandlineflag_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_commandlineflag_LIBS_DEBUG}"
                              "${abseil_absl_flags_commandlineflag_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_commandlineflag_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_commandlineflag_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_commandlineflag_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_commandlineflag_DEPS_TARGET
                              abseil_absl_flags_commandlineflag_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_commandlineflag"
                              "${abseil_absl_flags_commandlineflag_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_commandlineflag
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_commandlineflag_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_commandlineflag
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_commandlineflag_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_commandlineflag PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_commandlineflag PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_commandlineflag PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_commandlineflag PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_commandlineflag PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_path_util #############

        set(abseil_absl_flags_path_util_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_path_util_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_path_util_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_path_util_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_path_util_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_path_util_DEPS_TARGET)
            add_library(abseil_absl_flags_path_util_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_path_util_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_path_util_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_path_util_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_path_util_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_path_util_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_path_util_LIBS_DEBUG}"
                              "${abseil_absl_flags_path_util_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_path_util_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_path_util_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_path_util_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_path_util_DEPS_TARGET
                              abseil_absl_flags_path_util_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_path_util"
                              "${abseil_absl_flags_path_util_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_path_util
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_path_util_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_path_util_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_path_util_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_path_util
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_path_util_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_path_util PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_path_util_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_path_util PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_path_util_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_path_util PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_path_util_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_path_util PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_path_util_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_path_util PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_path_util_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::failure_signal_handler #############

        set(abseil_absl_failure_signal_handler_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_failure_signal_handler_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_failure_signal_handler_FRAMEWORKS_DEBUG}" "${abseil_absl_failure_signal_handler_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_failure_signal_handler_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_failure_signal_handler_DEPS_TARGET)
            add_library(abseil_absl_failure_signal_handler_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_failure_signal_handler_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_failure_signal_handler_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_failure_signal_handler_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_failure_signal_handler_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_failure_signal_handler_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_failure_signal_handler_LIBS_DEBUG}"
                              "${abseil_absl_failure_signal_handler_LIB_DIRS_DEBUG}"
                              "${abseil_absl_failure_signal_handler_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_failure_signal_handler_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_failure_signal_handler_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_failure_signal_handler_DEPS_TARGET
                              abseil_absl_failure_signal_handler_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_failure_signal_handler"
                              "${abseil_absl_failure_signal_handler_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::failure_signal_handler
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_failure_signal_handler_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_failure_signal_handler_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_failure_signal_handler_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::failure_signal_handler
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_failure_signal_handler_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::failure_signal_handler PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_failure_signal_handler_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::failure_signal_handler PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_failure_signal_handler_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::failure_signal_handler PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_failure_signal_handler_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::failure_signal_handler PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_failure_signal_handler_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::failure_signal_handler PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_failure_signal_handler_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::examine_stack #############

        set(abseil_absl_examine_stack_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_examine_stack_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_examine_stack_FRAMEWORKS_DEBUG}" "${abseil_absl_examine_stack_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_examine_stack_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_examine_stack_DEPS_TARGET)
            add_library(abseil_absl_examine_stack_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_examine_stack_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_examine_stack_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_examine_stack_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_examine_stack_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_examine_stack_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_examine_stack_LIBS_DEBUG}"
                              "${abseil_absl_examine_stack_LIB_DIRS_DEBUG}"
                              "${abseil_absl_examine_stack_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_examine_stack_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_examine_stack_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_examine_stack_DEPS_TARGET
                              abseil_absl_examine_stack_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_examine_stack"
                              "${abseil_absl_examine_stack_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::examine_stack
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_examine_stack_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_examine_stack_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_examine_stack_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::examine_stack
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_examine_stack_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::examine_stack PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_examine_stack_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::examine_stack PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_examine_stack_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::examine_stack PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_examine_stack_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::examine_stack PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_examine_stack_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::examine_stack PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_examine_stack_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::symbolize #############

        set(abseil_absl_symbolize_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_symbolize_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_symbolize_FRAMEWORKS_DEBUG}" "${abseil_absl_symbolize_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_symbolize_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_symbolize_DEPS_TARGET)
            add_library(abseil_absl_symbolize_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_symbolize_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_symbolize_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_symbolize_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_symbolize_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_symbolize_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_symbolize_LIBS_DEBUG}"
                              "${abseil_absl_symbolize_LIB_DIRS_DEBUG}"
                              "${abseil_absl_symbolize_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_symbolize_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_symbolize_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_symbolize_DEPS_TARGET
                              abseil_absl_symbolize_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_symbolize"
                              "${abseil_absl_symbolize_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::symbolize
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_symbolize_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_symbolize_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_symbolize_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::symbolize
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_symbolize_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::symbolize PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_symbolize_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::symbolize PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_symbolize_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::symbolize PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_symbolize_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::symbolize PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_symbolize_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::symbolize PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_symbolize_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::layout #############

        set(abseil_absl_layout_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_layout_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_layout_FRAMEWORKS_DEBUG}" "${abseil_absl_layout_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_layout_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_layout_DEPS_TARGET)
            add_library(abseil_absl_layout_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_layout_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_layout_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_layout_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_layout_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_layout_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_layout_LIBS_DEBUG}"
                              "${abseil_absl_layout_LIB_DIRS_DEBUG}"
                              "${abseil_absl_layout_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_layout_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_layout_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_layout_DEPS_TARGET
                              abseil_absl_layout_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_layout"
                              "${abseil_absl_layout_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::layout
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_layout_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_layout_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_layout_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::layout
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_layout_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::layout PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_layout_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::layout PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_layout_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::layout PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_layout_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::layout PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_layout_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::layout PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_layout_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::any #############

        set(abseil_absl_any_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_any_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_any_FRAMEWORKS_DEBUG}" "${abseil_absl_any_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_any_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_any_DEPS_TARGET)
            add_library(abseil_absl_any_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_any_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_any_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_any_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_any_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_any_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_any_LIBS_DEBUG}"
                              "${abseil_absl_any_LIB_DIRS_DEBUG}"
                              "${abseil_absl_any_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_any_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_any_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_any_DEPS_TARGET
                              abseil_absl_any_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_any"
                              "${abseil_absl_any_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::any
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_any_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_any_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_any_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::any
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_any_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::any PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_any_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::any PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_any_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::any PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_any_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::any PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_any_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::any PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_any_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::time #############

        set(abseil_absl_time_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_time_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_time_FRAMEWORKS_DEBUG}" "${abseil_absl_time_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_time_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_time_DEPS_TARGET)
            add_library(abseil_absl_time_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_time_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_time_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_time_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_time_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_time_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_time_LIBS_DEBUG}"
                              "${abseil_absl_time_LIB_DIRS_DEBUG}"
                              "${abseil_absl_time_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_time_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_time_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_time_DEPS_TARGET
                              abseil_absl_time_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_time"
                              "${abseil_absl_time_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::time
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_time_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_time_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_time_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::time
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_time_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::time PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_time_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::time PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_time_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::time PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_time_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::time PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_time_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::time PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_time_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::graphcycles_internal #############

        set(abseil_absl_graphcycles_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_graphcycles_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_graphcycles_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_graphcycles_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_graphcycles_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_graphcycles_internal_DEPS_TARGET)
            add_library(abseil_absl_graphcycles_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_graphcycles_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_graphcycles_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_graphcycles_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_graphcycles_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_graphcycles_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_graphcycles_internal_LIBS_DEBUG}"
                              "${abseil_absl_graphcycles_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_graphcycles_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_graphcycles_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_graphcycles_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_graphcycles_internal_DEPS_TARGET
                              abseil_absl_graphcycles_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_graphcycles_internal"
                              "${abseil_absl_graphcycles_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::graphcycles_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_graphcycles_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_graphcycles_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_graphcycles_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::graphcycles_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_graphcycles_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::graphcycles_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_graphcycles_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::graphcycles_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_graphcycles_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::graphcycles_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_graphcycles_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::graphcycles_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_graphcycles_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::graphcycles_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_graphcycles_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::str_format_internal #############

        set(abseil_absl_str_format_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_str_format_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_str_format_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_str_format_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_str_format_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_str_format_internal_DEPS_TARGET)
            add_library(abseil_absl_str_format_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_str_format_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_str_format_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_str_format_internal_LIBS_DEBUG}"
                              "${abseil_absl_str_format_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_str_format_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_str_format_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_str_format_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_str_format_internal_DEPS_TARGET
                              abseil_absl_str_format_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_str_format_internal"
                              "${abseil_absl_str_format_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::str_format_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_str_format_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::str_format_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_str_format_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::str_format_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::str_format_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::str_format_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::str_format_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::str_format_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_str_format_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::strings #############

        set(abseil_absl_strings_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_strings_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_strings_FRAMEWORKS_DEBUG}" "${abseil_absl_strings_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_strings_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_strings_DEPS_TARGET)
            add_library(abseil_absl_strings_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_strings_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_strings_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_strings_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_strings_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_strings_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_strings_LIBS_DEBUG}"
                              "${abseil_absl_strings_LIB_DIRS_DEBUG}"
                              "${abseil_absl_strings_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_strings_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_strings_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_strings_DEPS_TARGET
                              abseil_absl_strings_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_strings"
                              "${abseil_absl_strings_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::strings
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_strings_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_strings_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_strings_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::strings
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_strings_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::strings PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_strings_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::strings PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_strings_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::strings PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_strings_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::strings PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_strings_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::strings PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_strings_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_randen_engine #############

        set(abseil_absl_random_internal_randen_engine_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_randen_engine_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_randen_engine_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_randen_engine_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_randen_engine_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_randen_engine_DEPS_TARGET)
            add_library(abseil_absl_random_internal_randen_engine_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_randen_engine_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_engine_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_engine_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_engine_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_randen_engine_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_randen_engine_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_randen_engine_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_randen_engine_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_randen_engine_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_randen_engine_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_randen_engine_DEPS_TARGET
                              abseil_absl_random_internal_randen_engine_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_randen_engine"
                              "${abseil_absl_random_internal_randen_engine_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_randen_engine
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_engine_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_engine_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_randen_engine_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_randen_engine
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_randen_engine_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_randen_engine PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_engine_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_engine PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_engine_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_engine PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_engine_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_engine PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_engine_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_engine PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_engine_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_mock_helpers #############

        set(abseil_absl_random_internal_mock_helpers_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_mock_helpers_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_mock_helpers_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_mock_helpers_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_mock_helpers_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_mock_helpers_DEPS_TARGET)
            add_library(abseil_absl_random_internal_mock_helpers_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_mock_helpers_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_mock_helpers_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_mock_helpers_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_mock_helpers_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_mock_helpers_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_mock_helpers_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_mock_helpers_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_mock_helpers_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_mock_helpers_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_mock_helpers_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_mock_helpers_DEPS_TARGET
                              abseil_absl_random_internal_mock_helpers_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_mock_helpers"
                              "${abseil_absl_random_internal_mock_helpers_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_mock_helpers
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_mock_helpers_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_mock_helpers_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_mock_helpers_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_mock_helpers
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_mock_helpers_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_mock_helpers PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_mock_helpers_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_mock_helpers PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_mock_helpers_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_mock_helpers PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_mock_helpers_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_mock_helpers PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_mock_helpers_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_mock_helpers PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_mock_helpers_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_bit_gen_ref #############

        set(abseil_absl_random_bit_gen_ref_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_bit_gen_ref_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_bit_gen_ref_FRAMEWORKS_DEBUG}" "${abseil_absl_random_bit_gen_ref_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_bit_gen_ref_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_bit_gen_ref_DEPS_TARGET)
            add_library(abseil_absl_random_bit_gen_ref_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_bit_gen_ref_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_bit_gen_ref_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_bit_gen_ref_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_bit_gen_ref_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_bit_gen_ref_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_bit_gen_ref_LIBS_DEBUG}"
                              "${abseil_absl_random_bit_gen_ref_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_bit_gen_ref_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_bit_gen_ref_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_bit_gen_ref_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_bit_gen_ref_DEPS_TARGET
                              abseil_absl_random_bit_gen_ref_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_bit_gen_ref"
                              "${abseil_absl_random_bit_gen_ref_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_bit_gen_ref
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_bit_gen_ref_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_bit_gen_ref_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_bit_gen_ref_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_bit_gen_ref
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_bit_gen_ref_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_bit_gen_ref PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_bit_gen_ref_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_bit_gen_ref PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_bit_gen_ref_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_bit_gen_ref PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_bit_gen_ref_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_bit_gen_ref PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_bit_gen_ref_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_bit_gen_ref PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_bit_gen_ref_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::malloc_internal #############

        set(abseil_absl_malloc_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_malloc_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_malloc_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_malloc_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_malloc_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_malloc_internal_DEPS_TARGET)
            add_library(abseil_absl_malloc_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_malloc_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_malloc_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_malloc_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_malloc_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_malloc_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_malloc_internal_LIBS_DEBUG}"
                              "${abseil_absl_malloc_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_malloc_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_malloc_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_malloc_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_malloc_internal_DEPS_TARGET
                              abseil_absl_malloc_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_malloc_internal"
                              "${abseil_absl_malloc_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::malloc_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_malloc_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_malloc_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_malloc_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::malloc_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_malloc_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::malloc_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_malloc_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::malloc_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_malloc_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::malloc_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_malloc_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::malloc_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_malloc_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::malloc_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_malloc_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::variant #############

        set(abseil_absl_variant_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_variant_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_variant_FRAMEWORKS_DEBUG}" "${abseil_absl_variant_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_variant_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_variant_DEPS_TARGET)
            add_library(abseil_absl_variant_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_variant_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_variant_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_variant_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_variant_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_variant_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_variant_LIBS_DEBUG}"
                              "${abseil_absl_variant_LIB_DIRS_DEBUG}"
                              "${abseil_absl_variant_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_variant_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_variant_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_variant_DEPS_TARGET
                              abseil_absl_variant_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_variant"
                              "${abseil_absl_variant_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::variant
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_variant_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_variant_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_variant_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::variant
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_variant_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::variant PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_variant_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::variant PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_variant_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::variant PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_variant_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::variant PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_variant_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::variant PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_variant_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::optional #############

        set(abseil_absl_optional_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_optional_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_optional_FRAMEWORKS_DEBUG}" "${abseil_absl_optional_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_optional_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_optional_DEPS_TARGET)
            add_library(abseil_absl_optional_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_optional_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_optional_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_optional_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_optional_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_optional_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_optional_LIBS_DEBUG}"
                              "${abseil_absl_optional_LIB_DIRS_DEBUG}"
                              "${abseil_absl_optional_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_optional_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_optional_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_optional_DEPS_TARGET
                              abseil_absl_optional_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_optional"
                              "${abseil_absl_optional_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::optional
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_optional_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_optional_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_optional_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::optional
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_optional_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::optional PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_optional_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::optional PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_optional_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::optional PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_optional_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::optional PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_optional_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::optional PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_optional_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::bad_any_cast #############

        set(abseil_absl_bad_any_cast_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_bad_any_cast_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_bad_any_cast_FRAMEWORKS_DEBUG}" "${abseil_absl_bad_any_cast_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_bad_any_cast_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_bad_any_cast_DEPS_TARGET)
            add_library(abseil_absl_bad_any_cast_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_bad_any_cast_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_bad_any_cast_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_bad_any_cast_LIBS_DEBUG}"
                              "${abseil_absl_bad_any_cast_LIB_DIRS_DEBUG}"
                              "${abseil_absl_bad_any_cast_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_bad_any_cast_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_bad_any_cast_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_bad_any_cast_DEPS_TARGET
                              abseil_absl_bad_any_cast_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_bad_any_cast"
                              "${abseil_absl_bad_any_cast_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::bad_any_cast
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_bad_any_cast_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::bad_any_cast
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_bad_any_cast_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::bad_any_cast PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_any_cast PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_any_cast PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_any_cast PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_any_cast PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::strings_internal #############

        set(abseil_absl_strings_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_strings_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_strings_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_strings_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_strings_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_strings_internal_DEPS_TARGET)
            add_library(abseil_absl_strings_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_strings_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_strings_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_strings_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_strings_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_strings_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_strings_internal_LIBS_DEBUG}"
                              "${abseil_absl_strings_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_strings_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_strings_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_strings_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_strings_internal_DEPS_TARGET
                              abseil_absl_strings_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_strings_internal"
                              "${abseil_absl_strings_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::strings_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_strings_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_strings_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_strings_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::strings_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_strings_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::strings_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_strings_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::strings_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_strings_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::strings_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_strings_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::strings_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_strings_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::strings_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_strings_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_randen #############

        set(abseil_absl_random_internal_randen_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_randen_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_randen_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_randen_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_randen_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_randen_DEPS_TARGET)
            add_library(abseil_absl_random_internal_randen_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_randen_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_randen_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_randen_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_randen_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_randen_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_randen_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_randen_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_randen_DEPS_TARGET
                              abseil_absl_random_internal_randen_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_randen"
                              "${abseil_absl_random_internal_randen_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_randen
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_randen_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_randen
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_randen_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_randen PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_distribution_caller #############

        set(abseil_absl_random_internal_distribution_caller_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_distribution_caller_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_distribution_caller_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_distribution_caller_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_distribution_caller_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_distribution_caller_DEPS_TARGET)
            add_library(abseil_absl_random_internal_distribution_caller_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_distribution_caller_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_caller_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_caller_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_caller_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_distribution_caller_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_distribution_caller_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_distribution_caller_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_distribution_caller_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_distribution_caller_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_distribution_caller_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_distribution_caller_DEPS_TARGET
                              abseil_absl_random_internal_distribution_caller_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_distribution_caller"
                              "${abseil_absl_random_internal_distribution_caller_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_distribution_caller
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_caller_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_caller_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_distribution_caller_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_distribution_caller
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_distribution_caller_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_distribution_caller PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_caller_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_distribution_caller PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_caller_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_distribution_caller PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_caller_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_distribution_caller PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_caller_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_distribution_caller PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_distribution_caller_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::low_level_hash #############

        set(abseil_absl_low_level_hash_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_low_level_hash_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_low_level_hash_FRAMEWORKS_DEBUG}" "${abseil_absl_low_level_hash_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_low_level_hash_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_low_level_hash_DEPS_TARGET)
            add_library(abseil_absl_low_level_hash_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_low_level_hash_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_low_level_hash_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_low_level_hash_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_low_level_hash_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_low_level_hash_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_low_level_hash_LIBS_DEBUG}"
                              "${abseil_absl_low_level_hash_LIB_DIRS_DEBUG}"
                              "${abseil_absl_low_level_hash_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_low_level_hash_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_low_level_hash_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_low_level_hash_DEPS_TARGET
                              abseil_absl_low_level_hash_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_low_level_hash"
                              "${abseil_absl_low_level_hash_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::low_level_hash
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_low_level_hash_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_low_level_hash_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_low_level_hash_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::low_level_hash
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_low_level_hash_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::low_level_hash PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_low_level_hash_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::low_level_hash PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_low_level_hash_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::low_level_hash PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_low_level_hash_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::low_level_hash PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_low_level_hash_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::low_level_hash PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_low_level_hash_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::city #############

        set(abseil_absl_city_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_city_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_city_FRAMEWORKS_DEBUG}" "${abseil_absl_city_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_city_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_city_DEPS_TARGET)
            add_library(abseil_absl_city_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_city_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_city_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_city_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_city_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_city_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_city_LIBS_DEBUG}"
                              "${abseil_absl_city_LIB_DIRS_DEBUG}"
                              "${abseil_absl_city_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_city_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_city_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_city_DEPS_TARGET
                              abseil_absl_city_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_city"
                              "${abseil_absl_city_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::city
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_city_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_city_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_city_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::city
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_city_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::city PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_city_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::city PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_city_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::city PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_city_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::city PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_city_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::city PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_city_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::bind_front #############

        set(abseil_absl_bind_front_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_bind_front_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_bind_front_FRAMEWORKS_DEBUG}" "${abseil_absl_bind_front_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_bind_front_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_bind_front_DEPS_TARGET)
            add_library(abseil_absl_bind_front_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_bind_front_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bind_front_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bind_front_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bind_front_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_bind_front_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_bind_front_LIBS_DEBUG}"
                              "${abseil_absl_bind_front_LIB_DIRS_DEBUG}"
                              "${abseil_absl_bind_front_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_bind_front_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_bind_front_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_bind_front_DEPS_TARGET
                              abseil_absl_bind_front_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_bind_front"
                              "${abseil_absl_bind_front_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::bind_front
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bind_front_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bind_front_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_bind_front_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::bind_front
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_bind_front_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::bind_front PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bind_front_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::bind_front PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bind_front_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bind_front PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bind_front_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bind_front PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bind_front_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::bind_front PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bind_front_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::any_invocable #############

        set(abseil_absl_any_invocable_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_any_invocable_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_any_invocable_FRAMEWORKS_DEBUG}" "${abseil_absl_any_invocable_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_any_invocable_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_any_invocable_DEPS_TARGET)
            add_library(abseil_absl_any_invocable_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_any_invocable_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_any_invocable_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_any_invocable_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_any_invocable_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_any_invocable_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_any_invocable_LIBS_DEBUG}"
                              "${abseil_absl_any_invocable_LIB_DIRS_DEBUG}"
                              "${abseil_absl_any_invocable_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_any_invocable_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_any_invocable_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_any_invocable_DEPS_TARGET
                              abseil_absl_any_invocable_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_any_invocable"
                              "${abseil_absl_any_invocable_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::any_invocable
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_any_invocable_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_any_invocable_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_any_invocable_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::any_invocable
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_any_invocable_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::any_invocable PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_any_invocable_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::any_invocable PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_any_invocable_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::any_invocable PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_any_invocable_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::any_invocable PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_any_invocable_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::any_invocable PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_any_invocable_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::debugging #############

        set(abseil_absl_debugging_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_debugging_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_debugging_FRAMEWORKS_DEBUG}" "${abseil_absl_debugging_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_debugging_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_debugging_DEPS_TARGET)
            add_library(abseil_absl_debugging_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_debugging_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_debugging_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_debugging_LIBS_DEBUG}"
                              "${abseil_absl_debugging_LIB_DIRS_DEBUG}"
                              "${abseil_absl_debugging_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_debugging_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_debugging_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_debugging_DEPS_TARGET
                              abseil_absl_debugging_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_debugging"
                              "${abseil_absl_debugging_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::debugging
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_debugging_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::debugging
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_debugging_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::debugging PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::debugging PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::debugging PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::debugging PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::debugging PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::demangle_internal #############

        set(abseil_absl_demangle_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_demangle_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_demangle_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_demangle_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_demangle_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_demangle_internal_DEPS_TARGET)
            add_library(abseil_absl_demangle_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_demangle_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_demangle_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_demangle_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_demangle_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_demangle_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_demangle_internal_LIBS_DEBUG}"
                              "${abseil_absl_demangle_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_demangle_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_demangle_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_demangle_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_demangle_internal_DEPS_TARGET
                              abseil_absl_demangle_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_demangle_internal"
                              "${abseil_absl_demangle_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::demangle_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_demangle_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_demangle_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_demangle_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::demangle_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_demangle_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::demangle_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_demangle_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::demangle_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_demangle_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::demangle_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_demangle_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::demangle_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_demangle_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::demangle_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_demangle_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::stacktrace #############

        set(abseil_absl_stacktrace_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_stacktrace_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_stacktrace_FRAMEWORKS_DEBUG}" "${abseil_absl_stacktrace_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_stacktrace_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_stacktrace_DEPS_TARGET)
            add_library(abseil_absl_stacktrace_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_stacktrace_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_stacktrace_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_stacktrace_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_stacktrace_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_stacktrace_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_stacktrace_LIBS_DEBUG}"
                              "${abseil_absl_stacktrace_LIB_DIRS_DEBUG}"
                              "${abseil_absl_stacktrace_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_stacktrace_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_stacktrace_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_stacktrace_DEPS_TARGET
                              abseil_absl_stacktrace_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_stacktrace"
                              "${abseil_absl_stacktrace_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::stacktrace
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_stacktrace_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_stacktrace_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_stacktrace_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::stacktrace
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_stacktrace_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::stacktrace PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_stacktrace_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::stacktrace PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_stacktrace_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::stacktrace PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_stacktrace_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::stacktrace PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_stacktrace_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::stacktrace PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_stacktrace_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::container_memory #############

        set(abseil_absl_container_memory_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_container_memory_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_container_memory_FRAMEWORKS_DEBUG}" "${abseil_absl_container_memory_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_container_memory_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_container_memory_DEPS_TARGET)
            add_library(abseil_absl_container_memory_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_container_memory_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_container_memory_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_container_memory_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_container_memory_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_container_memory_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_container_memory_LIBS_DEBUG}"
                              "${abseil_absl_container_memory_LIB_DIRS_DEBUG}"
                              "${abseil_absl_container_memory_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_container_memory_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_container_memory_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_container_memory_DEPS_TARGET
                              abseil_absl_container_memory_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_container_memory"
                              "${abseil_absl_container_memory_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::container_memory
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_container_memory_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_container_memory_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_container_memory_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::container_memory
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_container_memory_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::container_memory PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_container_memory_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::container_memory PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_container_memory_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::container_memory PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_container_memory_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::container_memory PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_container_memory_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::container_memory PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_container_memory_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::inlined_vector #############

        set(abseil_absl_inlined_vector_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_inlined_vector_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_inlined_vector_FRAMEWORKS_DEBUG}" "${abseil_absl_inlined_vector_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_inlined_vector_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_inlined_vector_DEPS_TARGET)
            add_library(abseil_absl_inlined_vector_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_inlined_vector_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_inlined_vector_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_inlined_vector_LIBS_DEBUG}"
                              "${abseil_absl_inlined_vector_LIB_DIRS_DEBUG}"
                              "${abseil_absl_inlined_vector_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_inlined_vector_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_inlined_vector_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_inlined_vector_DEPS_TARGET
                              abseil_absl_inlined_vector_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_inlined_vector"
                              "${abseil_absl_inlined_vector_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::inlined_vector
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_inlined_vector_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::inlined_vector
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_inlined_vector_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::inlined_vector PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::inlined_vector PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::inlined_vector PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::inlined_vector PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::inlined_vector PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::inlined_vector_internal #############

        set(abseil_absl_inlined_vector_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_inlined_vector_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_inlined_vector_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_inlined_vector_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_inlined_vector_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_inlined_vector_internal_DEPS_TARGET)
            add_library(abseil_absl_inlined_vector_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_inlined_vector_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_inlined_vector_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_inlined_vector_internal_LIBS_DEBUG}"
                              "${abseil_absl_inlined_vector_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_inlined_vector_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_inlined_vector_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_inlined_vector_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_inlined_vector_internal_DEPS_TARGET
                              abseil_absl_inlined_vector_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_inlined_vector_internal"
                              "${abseil_absl_inlined_vector_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::inlined_vector_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_inlined_vector_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::inlined_vector_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_inlined_vector_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::inlined_vector_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::inlined_vector_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::inlined_vector_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::inlined_vector_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::inlined_vector_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_inlined_vector_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::fixed_array #############

        set(abseil_absl_fixed_array_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_fixed_array_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_fixed_array_FRAMEWORKS_DEBUG}" "${abseil_absl_fixed_array_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_fixed_array_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_fixed_array_DEPS_TARGET)
            add_library(abseil_absl_fixed_array_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_fixed_array_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_fixed_array_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_fixed_array_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_fixed_array_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_fixed_array_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_fixed_array_LIBS_DEBUG}"
                              "${abseil_absl_fixed_array_LIB_DIRS_DEBUG}"
                              "${abseil_absl_fixed_array_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_fixed_array_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_fixed_array_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_fixed_array_DEPS_TARGET
                              abseil_absl_fixed_array_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_fixed_array"
                              "${abseil_absl_fixed_array_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::fixed_array
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_fixed_array_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_fixed_array_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_fixed_array_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::fixed_array
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_fixed_array_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::fixed_array PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_fixed_array_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::fixed_array PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_fixed_array_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::fixed_array PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_fixed_array_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::fixed_array PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_fixed_array_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::fixed_array PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_fixed_array_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::compressed_tuple #############

        set(abseil_absl_compressed_tuple_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_compressed_tuple_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_compressed_tuple_FRAMEWORKS_DEBUG}" "${abseil_absl_compressed_tuple_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_compressed_tuple_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_compressed_tuple_DEPS_TARGET)
            add_library(abseil_absl_compressed_tuple_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_compressed_tuple_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_compressed_tuple_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_compressed_tuple_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_compressed_tuple_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_compressed_tuple_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_compressed_tuple_LIBS_DEBUG}"
                              "${abseil_absl_compressed_tuple_LIB_DIRS_DEBUG}"
                              "${abseil_absl_compressed_tuple_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_compressed_tuple_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_compressed_tuple_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_compressed_tuple_DEPS_TARGET
                              abseil_absl_compressed_tuple_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_compressed_tuple"
                              "${abseil_absl_compressed_tuple_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::compressed_tuple
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_compressed_tuple_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_compressed_tuple_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_compressed_tuple_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::compressed_tuple
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_compressed_tuple_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::compressed_tuple PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_compressed_tuple_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::compressed_tuple PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_compressed_tuple_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::compressed_tuple PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_compressed_tuple_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::compressed_tuple PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_compressed_tuple_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::compressed_tuple PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_compressed_tuple_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cleanup #############

        set(abseil_absl_cleanup_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cleanup_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cleanup_FRAMEWORKS_DEBUG}" "${abseil_absl_cleanup_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cleanup_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cleanup_DEPS_TARGET)
            add_library(abseil_absl_cleanup_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cleanup_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cleanup_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cleanup_LIBS_DEBUG}"
                              "${abseil_absl_cleanup_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cleanup_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cleanup_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cleanup_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cleanup_DEPS_TARGET
                              abseil_absl_cleanup_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cleanup"
                              "${abseil_absl_cleanup_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cleanup
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cleanup_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cleanup
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cleanup_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cleanup PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cleanup PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cleanup PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cleanup PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cleanup PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cleanup_internal #############

        set(abseil_absl_cleanup_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cleanup_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cleanup_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_cleanup_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cleanup_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cleanup_internal_DEPS_TARGET)
            add_library(abseil_absl_cleanup_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cleanup_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cleanup_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cleanup_internal_LIBS_DEBUG}"
                              "${abseil_absl_cleanup_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cleanup_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cleanup_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cleanup_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cleanup_internal_DEPS_TARGET
                              abseil_absl_cleanup_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cleanup_internal"
                              "${abseil_absl_cleanup_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cleanup_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cleanup_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cleanup_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cleanup_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cleanup_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cleanup_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cleanup_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cleanup_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cleanup_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cleanup_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::endian #############

        set(abseil_absl_endian_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_endian_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_endian_FRAMEWORKS_DEBUG}" "${abseil_absl_endian_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_endian_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_endian_DEPS_TARGET)
            add_library(abseil_absl_endian_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_endian_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_endian_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_endian_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_endian_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_endian_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_endian_LIBS_DEBUG}"
                              "${abseil_absl_endian_LIB_DIRS_DEBUG}"
                              "${abseil_absl_endian_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_endian_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_endian_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_endian_DEPS_TARGET
                              abseil_absl_endian_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_endian"
                              "${abseil_absl_endian_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::endian
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_endian_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_endian_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_endian_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::endian
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_endian_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::endian PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_endian_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::endian PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_endian_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::endian PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_endian_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::endian PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_endian_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::endian PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_endian_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::base #############

        set(abseil_absl_base_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_base_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_base_FRAMEWORKS_DEBUG}" "${abseil_absl_base_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_base_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_base_DEPS_TARGET)
            add_library(abseil_absl_base_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_base_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_base_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_base_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_base_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_base_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_base_LIBS_DEBUG}"
                              "${abseil_absl_base_LIB_DIRS_DEBUG}"
                              "${abseil_absl_base_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_base_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_base_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_base_DEPS_TARGET
                              abseil_absl_base_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_base"
                              "${abseil_absl_base_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::base
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_base_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_base_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_base_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::base
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_base_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::base PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_base_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::base PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_base_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::base PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_base_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::base PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_base_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::base PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_base_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::spinlock_wait #############

        set(abseil_absl_spinlock_wait_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_spinlock_wait_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_spinlock_wait_FRAMEWORKS_DEBUG}" "${abseil_absl_spinlock_wait_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_spinlock_wait_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_spinlock_wait_DEPS_TARGET)
            add_library(abseil_absl_spinlock_wait_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_spinlock_wait_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_spinlock_wait_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_spinlock_wait_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_spinlock_wait_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_spinlock_wait_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_spinlock_wait_LIBS_DEBUG}"
                              "${abseil_absl_spinlock_wait_LIB_DIRS_DEBUG}"
                              "${abseil_absl_spinlock_wait_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_spinlock_wait_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_spinlock_wait_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_spinlock_wait_DEPS_TARGET
                              abseil_absl_spinlock_wait_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_spinlock_wait"
                              "${abseil_absl_spinlock_wait_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::spinlock_wait
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_spinlock_wait_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_spinlock_wait_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_spinlock_wait_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::spinlock_wait
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_spinlock_wait_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::spinlock_wait PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_spinlock_wait_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::spinlock_wait PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_spinlock_wait_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::spinlock_wait PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_spinlock_wait_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::spinlock_wait PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_spinlock_wait_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::spinlock_wait PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_spinlock_wait_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::utility #############

        set(abseil_absl_utility_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_utility_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_utility_FRAMEWORKS_DEBUG}" "${abseil_absl_utility_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_utility_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_utility_DEPS_TARGET)
            add_library(abseil_absl_utility_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_utility_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_utility_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_utility_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_utility_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_utility_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_utility_LIBS_DEBUG}"
                              "${abseil_absl_utility_LIB_DIRS_DEBUG}"
                              "${abseil_absl_utility_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_utility_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_utility_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_utility_DEPS_TARGET
                              abseil_absl_utility_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_utility"
                              "${abseil_absl_utility_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::utility
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_utility_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_utility_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_utility_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::utility
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_utility_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::utility PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_utility_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::utility PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_utility_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::utility PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_utility_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::utility PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_utility_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::utility PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_utility_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::bad_variant_access #############

        set(abseil_absl_bad_variant_access_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_bad_variant_access_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_bad_variant_access_FRAMEWORKS_DEBUG}" "${abseil_absl_bad_variant_access_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_bad_variant_access_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_bad_variant_access_DEPS_TARGET)
            add_library(abseil_absl_bad_variant_access_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_bad_variant_access_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_variant_access_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_variant_access_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_variant_access_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_bad_variant_access_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_bad_variant_access_LIBS_DEBUG}"
                              "${abseil_absl_bad_variant_access_LIB_DIRS_DEBUG}"
                              "${abseil_absl_bad_variant_access_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_bad_variant_access_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_bad_variant_access_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_bad_variant_access_DEPS_TARGET
                              abseil_absl_bad_variant_access_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_bad_variant_access"
                              "${abseil_absl_bad_variant_access_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::bad_variant_access
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_variant_access_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_variant_access_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_bad_variant_access_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::bad_variant_access
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_bad_variant_access_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::bad_variant_access PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_variant_access_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_variant_access PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_variant_access_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_variant_access PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_variant_access_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_variant_access PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_variant_access_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_variant_access PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_variant_access_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::bad_optional_access #############

        set(abseil_absl_bad_optional_access_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_bad_optional_access_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_bad_optional_access_FRAMEWORKS_DEBUG}" "${abseil_absl_bad_optional_access_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_bad_optional_access_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_bad_optional_access_DEPS_TARGET)
            add_library(abseil_absl_bad_optional_access_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_bad_optional_access_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_optional_access_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_optional_access_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_optional_access_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_bad_optional_access_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_bad_optional_access_LIBS_DEBUG}"
                              "${abseil_absl_bad_optional_access_LIB_DIRS_DEBUG}"
                              "${abseil_absl_bad_optional_access_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_bad_optional_access_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_bad_optional_access_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_bad_optional_access_DEPS_TARGET
                              abseil_absl_bad_optional_access_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_bad_optional_access"
                              "${abseil_absl_bad_optional_access_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::bad_optional_access
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_optional_access_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_optional_access_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_bad_optional_access_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::bad_optional_access
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_bad_optional_access_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::bad_optional_access PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_optional_access_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_optional_access PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_optional_access_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_optional_access PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_optional_access_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_optional_access PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_optional_access_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_optional_access PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_optional_access_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::span #############

        set(abseil_absl_span_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_span_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_span_FRAMEWORKS_DEBUG}" "${abseil_absl_span_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_span_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_span_DEPS_TARGET)
            add_library(abseil_absl_span_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_span_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_span_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_span_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_span_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_span_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_span_LIBS_DEBUG}"
                              "${abseil_absl_span_LIB_DIRS_DEBUG}"
                              "${abseil_absl_span_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_span_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_span_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_span_DEPS_TARGET
                              abseil_absl_span_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_span"
                              "${abseil_absl_span_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::span
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_span_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_span_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_span_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::span
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_span_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::span PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_span_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::span PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_span_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::span PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_span_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::span PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_span_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::span PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_span_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::bad_any_cast_impl #############

        set(abseil_absl_bad_any_cast_impl_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_bad_any_cast_impl_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_bad_any_cast_impl_FRAMEWORKS_DEBUG}" "${abseil_absl_bad_any_cast_impl_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_bad_any_cast_impl_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_bad_any_cast_impl_DEPS_TARGET)
            add_library(abseil_absl_bad_any_cast_impl_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_bad_any_cast_impl_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_impl_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_impl_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_impl_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_bad_any_cast_impl_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_bad_any_cast_impl_LIBS_DEBUG}"
                              "${abseil_absl_bad_any_cast_impl_LIB_DIRS_DEBUG}"
                              "${abseil_absl_bad_any_cast_impl_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_bad_any_cast_impl_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_bad_any_cast_impl_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_bad_any_cast_impl_DEPS_TARGET
                              abseil_absl_bad_any_cast_impl_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_bad_any_cast_impl"
                              "${abseil_absl_bad_any_cast_impl_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::bad_any_cast_impl
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_impl_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_impl_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_bad_any_cast_impl_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::bad_any_cast_impl
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_bad_any_cast_impl_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::bad_any_cast_impl PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_impl_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_any_cast_impl PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_impl_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_any_cast_impl PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_impl_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_any_cast_impl PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_impl_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::bad_any_cast_impl PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bad_any_cast_impl_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cordz_functions #############

        set(abseil_absl_cordz_functions_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cordz_functions_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cordz_functions_FRAMEWORKS_DEBUG}" "${abseil_absl_cordz_functions_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cordz_functions_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cordz_functions_DEPS_TARGET)
            add_library(abseil_absl_cordz_functions_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cordz_functions_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_functions_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_functions_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_functions_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cordz_functions_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cordz_functions_LIBS_DEBUG}"
                              "${abseil_absl_cordz_functions_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cordz_functions_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cordz_functions_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cordz_functions_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cordz_functions_DEPS_TARGET
                              abseil_absl_cordz_functions_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cordz_functions"
                              "${abseil_absl_cordz_functions_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cordz_functions
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_functions_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_functions_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cordz_functions_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cordz_functions
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cordz_functions_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cordz_functions PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_functions_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_functions PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_functions_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_functions PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_functions_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_functions PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_functions_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_functions PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_functions_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_randen_hwaes #############

        set(abseil_absl_random_internal_randen_hwaes_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_randen_hwaes_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_randen_hwaes_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_randen_hwaes_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_randen_hwaes_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_randen_hwaes_DEPS_TARGET)
            add_library(abseil_absl_random_internal_randen_hwaes_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_randen_hwaes_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_randen_hwaes_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_randen_hwaes_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_randen_hwaes_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_randen_hwaes_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_randen_hwaes_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_randen_hwaes_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_randen_hwaes_DEPS_TARGET
                              abseil_absl_random_internal_randen_hwaes_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_randen_hwaes"
                              "${abseil_absl_random_internal_randen_hwaes_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_randen_hwaes
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_randen_hwaes_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_randen_hwaes
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_randen_hwaes_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_randen_hwaes PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_hwaes PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_hwaes PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_hwaes PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_hwaes PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_generate_real #############

        set(abseil_absl_random_internal_generate_real_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_generate_real_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_generate_real_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_generate_real_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_generate_real_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_generate_real_DEPS_TARGET)
            add_library(abseil_absl_random_internal_generate_real_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_generate_real_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_generate_real_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_generate_real_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_generate_real_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_generate_real_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_generate_real_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_generate_real_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_generate_real_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_generate_real_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_generate_real_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_generate_real_DEPS_TARGET
                              abseil_absl_random_internal_generate_real_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_generate_real"
                              "${abseil_absl_random_internal_generate_real_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_generate_real
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_generate_real_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_generate_real_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_generate_real_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_generate_real
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_generate_real_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_generate_real PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_generate_real_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_generate_real PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_generate_real_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_generate_real PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_generate_real_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_generate_real PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_generate_real_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_generate_real PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_generate_real_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::memory #############

        set(abseil_absl_memory_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_memory_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_memory_FRAMEWORKS_DEBUG}" "${abseil_absl_memory_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_memory_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_memory_DEPS_TARGET)
            add_library(abseil_absl_memory_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_memory_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_memory_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_memory_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_memory_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_memory_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_memory_LIBS_DEBUG}"
                              "${abseil_absl_memory_LIB_DIRS_DEBUG}"
                              "${abseil_absl_memory_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_memory_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_memory_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_memory_DEPS_TARGET
                              abseil_absl_memory_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_memory"
                              "${abseil_absl_memory_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::memory
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_memory_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_memory_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_memory_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::memory
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_memory_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::memory PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_memory_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::memory PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_memory_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::memory PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_memory_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::memory PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_memory_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::memory PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_memory_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::function_ref #############

        set(abseil_absl_function_ref_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_function_ref_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_function_ref_FRAMEWORKS_DEBUG}" "${abseil_absl_function_ref_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_function_ref_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_function_ref_DEPS_TARGET)
            add_library(abseil_absl_function_ref_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_function_ref_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_function_ref_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_function_ref_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_function_ref_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_function_ref_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_function_ref_LIBS_DEBUG}"
                              "${abseil_absl_function_ref_LIB_DIRS_DEBUG}"
                              "${abseil_absl_function_ref_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_function_ref_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_function_ref_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_function_ref_DEPS_TARGET
                              abseil_absl_function_ref_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_function_ref"
                              "${abseil_absl_function_ref_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::function_ref
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_function_ref_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_function_ref_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_function_ref_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::function_ref
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_function_ref_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::function_ref PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_function_ref_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::function_ref PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_function_ref_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::function_ref PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_function_ref_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::function_ref PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_function_ref_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::function_ref PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_function_ref_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::debugging_internal #############

        set(abseil_absl_debugging_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_debugging_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_debugging_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_debugging_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_debugging_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_debugging_internal_DEPS_TARGET)
            add_library(abseil_absl_debugging_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_debugging_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_debugging_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_debugging_internal_LIBS_DEBUG}"
                              "${abseil_absl_debugging_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_debugging_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_debugging_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_debugging_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_debugging_internal_DEPS_TARGET
                              abseil_absl_debugging_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_debugging_internal"
                              "${abseil_absl_debugging_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::debugging_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_debugging_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::debugging_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_debugging_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::debugging_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::debugging_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::debugging_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::debugging_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::debugging_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_debugging_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::container_common #############

        set(abseil_absl_container_common_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_container_common_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_container_common_FRAMEWORKS_DEBUG}" "${abseil_absl_container_common_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_container_common_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_container_common_DEPS_TARGET)
            add_library(abseil_absl_container_common_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_container_common_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_container_common_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_container_common_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_container_common_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_container_common_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_container_common_LIBS_DEBUG}"
                              "${abseil_absl_container_common_LIB_DIRS_DEBUG}"
                              "${abseil_absl_container_common_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_container_common_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_container_common_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_container_common_DEPS_TARGET
                              abseil_absl_container_common_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_container_common"
                              "${abseil_absl_container_common_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::container_common
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_container_common_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_container_common_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_container_common_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::container_common
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_container_common_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::container_common PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_container_common_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::container_common PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_container_common_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::container_common PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_container_common_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::container_common PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_container_common_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::container_common PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_container_common_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::hashtable_debug #############

        set(abseil_absl_hashtable_debug_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_hashtable_debug_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_hashtable_debug_FRAMEWORKS_DEBUG}" "${abseil_absl_hashtable_debug_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_hashtable_debug_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_hashtable_debug_DEPS_TARGET)
            add_library(abseil_absl_hashtable_debug_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_hashtable_debug_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_hashtable_debug_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_hashtable_debug_LIBS_DEBUG}"
                              "${abseil_absl_hashtable_debug_LIB_DIRS_DEBUG}"
                              "${abseil_absl_hashtable_debug_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_hashtable_debug_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_hashtable_debug_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_hashtable_debug_DEPS_TARGET
                              abseil_absl_hashtable_debug_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_hashtable_debug"
                              "${abseil_absl_hashtable_debug_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::hashtable_debug
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_hashtable_debug_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::hashtable_debug
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_hashtable_debug_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::hashtable_debug PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtable_debug PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtable_debug PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtable_debug PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtable_debug PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::hash_policy_traits #############

        set(abseil_absl_hash_policy_traits_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_hash_policy_traits_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_hash_policy_traits_FRAMEWORKS_DEBUG}" "${abseil_absl_hash_policy_traits_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_hash_policy_traits_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_hash_policy_traits_DEPS_TARGET)
            add_library(abseil_absl_hash_policy_traits_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_hash_policy_traits_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_policy_traits_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hash_policy_traits_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hash_policy_traits_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_hash_policy_traits_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_hash_policy_traits_LIBS_DEBUG}"
                              "${abseil_absl_hash_policy_traits_LIB_DIRS_DEBUG}"
                              "${abseil_absl_hash_policy_traits_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_hash_policy_traits_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_hash_policy_traits_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_hash_policy_traits_DEPS_TARGET
                              abseil_absl_hash_policy_traits_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_hash_policy_traits"
                              "${abseil_absl_hash_policy_traits_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::hash_policy_traits
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_policy_traits_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hash_policy_traits_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_hash_policy_traits_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::hash_policy_traits
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_hash_policy_traits_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::hash_policy_traits PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hash_policy_traits_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::hash_policy_traits PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_policy_traits_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hash_policy_traits PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hash_policy_traits_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hash_policy_traits PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hash_policy_traits_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::hash_policy_traits PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hash_policy_traits_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::algorithm_container #############

        set(abseil_absl_algorithm_container_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_algorithm_container_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_algorithm_container_FRAMEWORKS_DEBUG}" "${abseil_absl_algorithm_container_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_algorithm_container_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_algorithm_container_DEPS_TARGET)
            add_library(abseil_absl_algorithm_container_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_algorithm_container_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_container_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_container_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_container_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_algorithm_container_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_algorithm_container_LIBS_DEBUG}"
                              "${abseil_absl_algorithm_container_LIB_DIRS_DEBUG}"
                              "${abseil_absl_algorithm_container_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_algorithm_container_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_algorithm_container_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_algorithm_container_DEPS_TARGET
                              abseil_absl_algorithm_container_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_algorithm_container"
                              "${abseil_absl_algorithm_container_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::algorithm_container
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_container_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_container_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_algorithm_container_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::algorithm_container
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_algorithm_container_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::algorithm_container PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_container_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::algorithm_container PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_container_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::algorithm_container PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_container_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::algorithm_container PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_container_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::algorithm_container PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_container_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::strerror #############

        set(abseil_absl_strerror_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_strerror_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_strerror_FRAMEWORKS_DEBUG}" "${abseil_absl_strerror_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_strerror_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_strerror_DEPS_TARGET)
            add_library(abseil_absl_strerror_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_strerror_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_strerror_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_strerror_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_strerror_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_strerror_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_strerror_LIBS_DEBUG}"
                              "${abseil_absl_strerror_LIB_DIRS_DEBUG}"
                              "${abseil_absl_strerror_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_strerror_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_strerror_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_strerror_DEPS_TARGET
                              abseil_absl_strerror_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_strerror"
                              "${abseil_absl_strerror_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::strerror
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_strerror_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_strerror_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_strerror_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::strerror
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_strerror_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::strerror PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_strerror_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::strerror PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_strerror_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::strerror PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_strerror_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::strerror PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_strerror_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::strerror PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_strerror_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::scoped_set_env #############

        set(abseil_absl_scoped_set_env_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_scoped_set_env_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_scoped_set_env_FRAMEWORKS_DEBUG}" "${abseil_absl_scoped_set_env_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_scoped_set_env_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_scoped_set_env_DEPS_TARGET)
            add_library(abseil_absl_scoped_set_env_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_scoped_set_env_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_scoped_set_env_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_scoped_set_env_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_scoped_set_env_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_scoped_set_env_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_scoped_set_env_LIBS_DEBUG}"
                              "${abseil_absl_scoped_set_env_LIB_DIRS_DEBUG}"
                              "${abseil_absl_scoped_set_env_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_scoped_set_env_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_scoped_set_env_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_scoped_set_env_DEPS_TARGET
                              abseil_absl_scoped_set_env_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_scoped_set_env"
                              "${abseil_absl_scoped_set_env_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::scoped_set_env
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_scoped_set_env_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_scoped_set_env_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_scoped_set_env_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::scoped_set_env
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_scoped_set_env_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::scoped_set_env PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_scoped_set_env_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::scoped_set_env PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_scoped_set_env_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::scoped_set_env PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_scoped_set_env_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::scoped_set_env PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_scoped_set_env_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::scoped_set_env PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_scoped_set_env_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::throw_delegate #############

        set(abseil_absl_throw_delegate_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_throw_delegate_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_throw_delegate_FRAMEWORKS_DEBUG}" "${abseil_absl_throw_delegate_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_throw_delegate_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_throw_delegate_DEPS_TARGET)
            add_library(abseil_absl_throw_delegate_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_throw_delegate_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_throw_delegate_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_throw_delegate_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_throw_delegate_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_throw_delegate_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_throw_delegate_LIBS_DEBUG}"
                              "${abseil_absl_throw_delegate_LIB_DIRS_DEBUG}"
                              "${abseil_absl_throw_delegate_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_throw_delegate_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_throw_delegate_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_throw_delegate_DEPS_TARGET
                              abseil_absl_throw_delegate_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_throw_delegate"
                              "${abseil_absl_throw_delegate_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::throw_delegate
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_throw_delegate_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_throw_delegate_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_throw_delegate_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::throw_delegate
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_throw_delegate_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::throw_delegate PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_throw_delegate_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::throw_delegate PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_throw_delegate_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::throw_delegate PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_throw_delegate_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::throw_delegate PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_throw_delegate_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::throw_delegate PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_throw_delegate_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::base_internal #############

        set(abseil_absl_base_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_base_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_base_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_base_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_base_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_base_internal_DEPS_TARGET)
            add_library(abseil_absl_base_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_base_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_base_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_base_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_base_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_base_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_base_internal_LIBS_DEBUG}"
                              "${abseil_absl_base_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_base_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_base_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_base_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_base_internal_DEPS_TARGET
                              abseil_absl_base_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_base_internal"
                              "${abseil_absl_base_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::base_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_base_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_base_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_base_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::base_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_base_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::base_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_base_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::base_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_base_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::base_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_base_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::base_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_base_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::base_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_base_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::raw_logging_internal #############

        set(abseil_absl_raw_logging_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_raw_logging_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_raw_logging_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_raw_logging_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_raw_logging_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_raw_logging_internal_DEPS_TARGET)
            add_library(abseil_absl_raw_logging_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_raw_logging_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_logging_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_raw_logging_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_raw_logging_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_raw_logging_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_raw_logging_internal_LIBS_DEBUG}"
                              "${abseil_absl_raw_logging_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_raw_logging_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_raw_logging_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_raw_logging_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_raw_logging_internal_DEPS_TARGET
                              abseil_absl_raw_logging_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_raw_logging_internal"
                              "${abseil_absl_raw_logging_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::raw_logging_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_logging_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_raw_logging_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_raw_logging_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::raw_logging_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_raw_logging_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::raw_logging_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_raw_logging_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_logging_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_logging_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_logging_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_raw_logging_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_logging_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_raw_logging_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::raw_logging_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_raw_logging_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::log_severity #############

        set(abseil_absl_log_severity_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_log_severity_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_log_severity_FRAMEWORKS_DEBUG}" "${abseil_absl_log_severity_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_log_severity_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_log_severity_DEPS_TARGET)
            add_library(abseil_absl_log_severity_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_log_severity_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_log_severity_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_log_severity_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_log_severity_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_log_severity_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_log_severity_LIBS_DEBUG}"
                              "${abseil_absl_log_severity_LIB_DIRS_DEBUG}"
                              "${abseil_absl_log_severity_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_log_severity_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_log_severity_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_log_severity_DEPS_TARGET
                              abseil_absl_log_severity_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_log_severity"
                              "${abseil_absl_log_severity_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::log_severity
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_log_severity_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_log_severity_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_log_severity_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::log_severity
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_log_severity_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::log_severity PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_log_severity_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::log_severity PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_log_severity_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::log_severity PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_log_severity_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::log_severity PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_log_severity_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::log_severity PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_log_severity_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::errno_saver #############

        set(abseil_absl_errno_saver_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_errno_saver_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_errno_saver_FRAMEWORKS_DEBUG}" "${abseil_absl_errno_saver_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_errno_saver_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_errno_saver_DEPS_TARGET)
            add_library(abseil_absl_errno_saver_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_errno_saver_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_errno_saver_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_errno_saver_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_errno_saver_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_errno_saver_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_errno_saver_LIBS_DEBUG}"
                              "${abseil_absl_errno_saver_LIB_DIRS_DEBUG}"
                              "${abseil_absl_errno_saver_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_errno_saver_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_errno_saver_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_errno_saver_DEPS_TARGET
                              abseil_absl_errno_saver_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_errno_saver"
                              "${abseil_absl_errno_saver_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::errno_saver
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_errno_saver_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_errno_saver_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_errno_saver_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::errno_saver
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_errno_saver_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::errno_saver PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_errno_saver_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::errno_saver PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_errno_saver_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::errno_saver PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_errno_saver_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::errno_saver PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_errno_saver_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::errno_saver PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_errno_saver_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::atomic_hook #############

        set(abseil_absl_atomic_hook_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_atomic_hook_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_atomic_hook_FRAMEWORKS_DEBUG}" "${abseil_absl_atomic_hook_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_atomic_hook_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_atomic_hook_DEPS_TARGET)
            add_library(abseil_absl_atomic_hook_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_atomic_hook_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_atomic_hook_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_atomic_hook_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_atomic_hook_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_atomic_hook_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_atomic_hook_LIBS_DEBUG}"
                              "${abseil_absl_atomic_hook_LIB_DIRS_DEBUG}"
                              "${abseil_absl_atomic_hook_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_atomic_hook_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_atomic_hook_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_atomic_hook_DEPS_TARGET
                              abseil_absl_atomic_hook_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_atomic_hook"
                              "${abseil_absl_atomic_hook_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::atomic_hook
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_atomic_hook_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_atomic_hook_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_atomic_hook_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::atomic_hook
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_atomic_hook_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::atomic_hook PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_atomic_hook_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::atomic_hook PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_atomic_hook_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::atomic_hook PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_atomic_hook_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::atomic_hook PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_atomic_hook_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::atomic_hook PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_atomic_hook_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::compare #############

        set(abseil_absl_compare_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_compare_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_compare_FRAMEWORKS_DEBUG}" "${abseil_absl_compare_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_compare_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_compare_DEPS_TARGET)
            add_library(abseil_absl_compare_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_compare_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_compare_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_compare_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_compare_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_compare_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_compare_LIBS_DEBUG}"
                              "${abseil_absl_compare_LIB_DIRS_DEBUG}"
                              "${abseil_absl_compare_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_compare_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_compare_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_compare_DEPS_TARGET
                              abseil_absl_compare_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_compare"
                              "${abseil_absl_compare_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::compare
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_compare_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_compare_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_compare_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::compare
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_compare_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::compare PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_compare_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::compare PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_compare_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::compare PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_compare_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::compare PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_compare_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::compare PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_compare_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::time_zone #############

        set(abseil_absl_time_zone_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_time_zone_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_time_zone_FRAMEWORKS_DEBUG}" "${abseil_absl_time_zone_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_time_zone_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_time_zone_DEPS_TARGET)
            add_library(abseil_absl_time_zone_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_time_zone_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_time_zone_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_time_zone_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_time_zone_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_time_zone_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_time_zone_LIBS_DEBUG}"
                              "${abseil_absl_time_zone_LIB_DIRS_DEBUG}"
                              "${abseil_absl_time_zone_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_time_zone_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_time_zone_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_time_zone_DEPS_TARGET
                              abseil_absl_time_zone_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_time_zone"
                              "${abseil_absl_time_zone_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::time_zone
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_time_zone_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_time_zone_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_time_zone_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::time_zone
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_time_zone_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::time_zone PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_time_zone_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::time_zone PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_time_zone_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::time_zone PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_time_zone_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::time_zone PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_time_zone_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::time_zone PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_time_zone_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::civil_time #############

        set(abseil_absl_civil_time_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_civil_time_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_civil_time_FRAMEWORKS_DEBUG}" "${abseil_absl_civil_time_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_civil_time_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_civil_time_DEPS_TARGET)
            add_library(abseil_absl_civil_time_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_civil_time_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_civil_time_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_civil_time_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_civil_time_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_civil_time_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_civil_time_LIBS_DEBUG}"
                              "${abseil_absl_civil_time_LIB_DIRS_DEBUG}"
                              "${abseil_absl_civil_time_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_civil_time_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_civil_time_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_civil_time_DEPS_TARGET
                              abseil_absl_civil_time_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_civil_time"
                              "${abseil_absl_civil_time_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::civil_time
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_civil_time_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_civil_time_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_civil_time_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::civil_time
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_civil_time_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::civil_time PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_civil_time_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::civil_time PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_civil_time_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::civil_time PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_civil_time_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::civil_time PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_civil_time_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::civil_time PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_civil_time_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::cordz_update_tracker #############

        set(abseil_absl_cordz_update_tracker_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_cordz_update_tracker_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_cordz_update_tracker_FRAMEWORKS_DEBUG}" "${abseil_absl_cordz_update_tracker_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_cordz_update_tracker_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_cordz_update_tracker_DEPS_TARGET)
            add_library(abseil_absl_cordz_update_tracker_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_cordz_update_tracker_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_tracker_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_tracker_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_tracker_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_cordz_update_tracker_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_cordz_update_tracker_LIBS_DEBUG}"
                              "${abseil_absl_cordz_update_tracker_LIB_DIRS_DEBUG}"
                              "${abseil_absl_cordz_update_tracker_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_cordz_update_tracker_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_cordz_update_tracker_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_cordz_update_tracker_DEPS_TARGET
                              abseil_absl_cordz_update_tracker_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_cordz_update_tracker"
                              "${abseil_absl_cordz_update_tracker_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::cordz_update_tracker
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_tracker_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_tracker_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_cordz_update_tracker_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::cordz_update_tracker
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_cordz_update_tracker_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::cordz_update_tracker PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_tracker_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_update_tracker PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_tracker_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_update_tracker PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_tracker_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_update_tracker PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_tracker_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::cordz_update_tracker PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_cordz_update_tracker_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_uniform_helper #############

        set(abseil_absl_random_internal_uniform_helper_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_uniform_helper_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_uniform_helper_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_uniform_helper_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_uniform_helper_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_uniform_helper_DEPS_TARGET)
            add_library(abseil_absl_random_internal_uniform_helper_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_uniform_helper_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_uniform_helper_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_uniform_helper_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_uniform_helper_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_uniform_helper_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_uniform_helper_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_uniform_helper_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_uniform_helper_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_uniform_helper_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_uniform_helper_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_uniform_helper_DEPS_TARGET
                              abseil_absl_random_internal_uniform_helper_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_uniform_helper"
                              "${abseil_absl_random_internal_uniform_helper_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_uniform_helper
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_uniform_helper_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_uniform_helper_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_uniform_helper_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_uniform_helper
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_uniform_helper_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_uniform_helper PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_uniform_helper_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_uniform_helper PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_uniform_helper_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_uniform_helper PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_uniform_helper_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_uniform_helper PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_uniform_helper_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_uniform_helper PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_uniform_helper_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_randen_hwaes_impl #############

        set(abseil_absl_random_internal_randen_hwaes_impl_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_randen_hwaes_impl_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_randen_hwaes_impl_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_randen_hwaes_impl_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_randen_hwaes_impl_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_randen_hwaes_impl_DEPS_TARGET)
            add_library(abseil_absl_random_internal_randen_hwaes_impl_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_randen_hwaes_impl_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_impl_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_impl_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_impl_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_randen_hwaes_impl_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_randen_hwaes_impl_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_randen_hwaes_impl_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_randen_hwaes_impl_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_randen_hwaes_impl_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_randen_hwaes_impl_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_randen_hwaes_impl_DEPS_TARGET
                              abseil_absl_random_internal_randen_hwaes_impl_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_randen_hwaes_impl"
                              "${abseil_absl_random_internal_randen_hwaes_impl_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_randen_hwaes_impl
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_impl_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_impl_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_randen_hwaes_impl_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_randen_hwaes_impl
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_randen_hwaes_impl_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_randen_hwaes_impl PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_impl_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_hwaes_impl PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_impl_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_hwaes_impl PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_impl_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_hwaes_impl PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_impl_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_hwaes_impl PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_hwaes_impl_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_randen_slow #############

        set(abseil_absl_random_internal_randen_slow_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_randen_slow_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_randen_slow_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_randen_slow_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_randen_slow_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_randen_slow_DEPS_TARGET)
            add_library(abseil_absl_random_internal_randen_slow_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_randen_slow_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_slow_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_slow_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_slow_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_randen_slow_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_randen_slow_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_randen_slow_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_randen_slow_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_randen_slow_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_randen_slow_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_randen_slow_DEPS_TARGET
                              abseil_absl_random_internal_randen_slow_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_randen_slow"
                              "${abseil_absl_random_internal_randen_slow_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_randen_slow
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_slow_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_slow_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_randen_slow_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_randen_slow
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_randen_slow_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_randen_slow PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_slow_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_slow PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_slow_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_slow PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_slow_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_slow PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_slow_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_randen_slow PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_randen_slow_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_platform #############

        set(abseil_absl_random_internal_platform_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_platform_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_platform_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_platform_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_platform_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_platform_DEPS_TARGET)
            add_library(abseil_absl_random_internal_platform_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_platform_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_platform_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_platform_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_platform_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_platform_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_platform_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_platform_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_platform_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_platform_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_platform_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_platform_DEPS_TARGET
                              abseil_absl_random_internal_platform_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_platform"
                              "${abseil_absl_random_internal_platform_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_platform
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_platform_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_platform_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_platform_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_platform
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_platform_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_platform PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_platform_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_platform PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_platform_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_platform PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_platform_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_platform PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_platform_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_platform PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_platform_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_pcg_engine #############

        set(abseil_absl_random_internal_pcg_engine_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_pcg_engine_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_pcg_engine_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_pcg_engine_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_pcg_engine_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_pcg_engine_DEPS_TARGET)
            add_library(abseil_absl_random_internal_pcg_engine_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_pcg_engine_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pcg_engine_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pcg_engine_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pcg_engine_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_pcg_engine_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_pcg_engine_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_pcg_engine_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_pcg_engine_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_pcg_engine_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_pcg_engine_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_pcg_engine_DEPS_TARGET
                              abseil_absl_random_internal_pcg_engine_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_pcg_engine"
                              "${abseil_absl_random_internal_pcg_engine_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_pcg_engine
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pcg_engine_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pcg_engine_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_pcg_engine_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_pcg_engine
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_pcg_engine_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_pcg_engine PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pcg_engine_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_pcg_engine PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pcg_engine_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_pcg_engine PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pcg_engine_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_pcg_engine PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pcg_engine_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_pcg_engine PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_pcg_engine_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_fastmath #############

        set(abseil_absl_random_internal_fastmath_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_fastmath_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_fastmath_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_fastmath_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_fastmath_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_fastmath_DEPS_TARGET)
            add_library(abseil_absl_random_internal_fastmath_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_fastmath_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fastmath_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fastmath_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fastmath_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_fastmath_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_fastmath_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_fastmath_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_fastmath_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_fastmath_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_fastmath_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_fastmath_DEPS_TARGET
                              abseil_absl_random_internal_fastmath_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_fastmath"
                              "${abseil_absl_random_internal_fastmath_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_fastmath
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fastmath_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fastmath_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_fastmath_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_fastmath
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_fastmath_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_fastmath PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fastmath_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_fastmath PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fastmath_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_fastmath PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fastmath_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_fastmath PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fastmath_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_fastmath PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fastmath_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_wide_multiply #############

        set(abseil_absl_random_internal_wide_multiply_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_wide_multiply_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_wide_multiply_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_wide_multiply_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_wide_multiply_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_wide_multiply_DEPS_TARGET)
            add_library(abseil_absl_random_internal_wide_multiply_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_wide_multiply_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_wide_multiply_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_wide_multiply_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_wide_multiply_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_wide_multiply_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_wide_multiply_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_wide_multiply_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_wide_multiply_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_wide_multiply_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_wide_multiply_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_wide_multiply_DEPS_TARGET
                              abseil_absl_random_internal_wide_multiply_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_wide_multiply"
                              "${abseil_absl_random_internal_wide_multiply_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_wide_multiply
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_wide_multiply_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_wide_multiply_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_wide_multiply_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_wide_multiply
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_wide_multiply_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_wide_multiply PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_wide_multiply_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_wide_multiply PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_wide_multiply_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_wide_multiply PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_wide_multiply_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_wide_multiply PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_wide_multiply_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_wide_multiply PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_wide_multiply_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_iostream_state_saver #############

        set(abseil_absl_random_internal_iostream_state_saver_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_iostream_state_saver_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_iostream_state_saver_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_iostream_state_saver_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_iostream_state_saver_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_iostream_state_saver_DEPS_TARGET)
            add_library(abseil_absl_random_internal_iostream_state_saver_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_iostream_state_saver_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_iostream_state_saver_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_iostream_state_saver_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_iostream_state_saver_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_iostream_state_saver_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_iostream_state_saver_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_iostream_state_saver_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_iostream_state_saver_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_iostream_state_saver_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_iostream_state_saver_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_iostream_state_saver_DEPS_TARGET
                              abseil_absl_random_internal_iostream_state_saver_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_iostream_state_saver"
                              "${abseil_absl_random_internal_iostream_state_saver_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_iostream_state_saver
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_iostream_state_saver_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_iostream_state_saver_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_iostream_state_saver_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_iostream_state_saver
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_iostream_state_saver_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_iostream_state_saver PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_iostream_state_saver_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_iostream_state_saver PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_iostream_state_saver_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_iostream_state_saver PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_iostream_state_saver_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_iostream_state_saver PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_iostream_state_saver_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_iostream_state_saver PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_iostream_state_saver_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_fast_uniform_bits #############

        set(abseil_absl_random_internal_fast_uniform_bits_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_fast_uniform_bits_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_fast_uniform_bits_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_fast_uniform_bits_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_fast_uniform_bits_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_fast_uniform_bits_DEPS_TARGET)
            add_library(abseil_absl_random_internal_fast_uniform_bits_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_fast_uniform_bits_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fast_uniform_bits_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fast_uniform_bits_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fast_uniform_bits_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_fast_uniform_bits_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_fast_uniform_bits_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_fast_uniform_bits_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_fast_uniform_bits_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_fast_uniform_bits_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_fast_uniform_bits_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_fast_uniform_bits_DEPS_TARGET
                              abseil_absl_random_internal_fast_uniform_bits_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_fast_uniform_bits"
                              "${abseil_absl_random_internal_fast_uniform_bits_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_fast_uniform_bits
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fast_uniform_bits_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fast_uniform_bits_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_fast_uniform_bits_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_fast_uniform_bits
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_fast_uniform_bits_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_fast_uniform_bits PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fast_uniform_bits_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_fast_uniform_bits PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fast_uniform_bits_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_fast_uniform_bits PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fast_uniform_bits_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_fast_uniform_bits PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fast_uniform_bits_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_fast_uniform_bits PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_fast_uniform_bits_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_internal_traits #############

        set(abseil_absl_random_internal_traits_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_internal_traits_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_internal_traits_FRAMEWORKS_DEBUG}" "${abseil_absl_random_internal_traits_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_internal_traits_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_internal_traits_DEPS_TARGET)
            add_library(abseil_absl_random_internal_traits_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_internal_traits_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_traits_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_traits_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_traits_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_internal_traits_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_internal_traits_LIBS_DEBUG}"
                              "${abseil_absl_random_internal_traits_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_internal_traits_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_internal_traits_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_internal_traits_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_internal_traits_DEPS_TARGET
                              abseil_absl_random_internal_traits_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_internal_traits"
                              "${abseil_absl_random_internal_traits_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_internal_traits
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_traits_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_traits_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_internal_traits_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_internal_traits
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_internal_traits_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_internal_traits PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_traits_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_traits PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_traits_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_traits PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_traits_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_traits PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_traits_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_internal_traits PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_internal_traits_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::random_seed_gen_exception #############

        set(abseil_absl_random_seed_gen_exception_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_random_seed_gen_exception_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_random_seed_gen_exception_FRAMEWORKS_DEBUG}" "${abseil_absl_random_seed_gen_exception_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_random_seed_gen_exception_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_random_seed_gen_exception_DEPS_TARGET)
            add_library(abseil_absl_random_seed_gen_exception_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_random_seed_gen_exception_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_gen_exception_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_gen_exception_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_gen_exception_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_random_seed_gen_exception_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_random_seed_gen_exception_LIBS_DEBUG}"
                              "${abseil_absl_random_seed_gen_exception_LIB_DIRS_DEBUG}"
                              "${abseil_absl_random_seed_gen_exception_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_random_seed_gen_exception_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_random_seed_gen_exception_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_random_seed_gen_exception_DEPS_TARGET
                              abseil_absl_random_seed_gen_exception_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_random_seed_gen_exception"
                              "${abseil_absl_random_seed_gen_exception_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::random_seed_gen_exception
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_gen_exception_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_gen_exception_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_random_seed_gen_exception_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::random_seed_gen_exception
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_random_seed_gen_exception_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::random_seed_gen_exception PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_gen_exception_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::random_seed_gen_exception PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_gen_exception_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_seed_gen_exception PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_gen_exception_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::random_seed_gen_exception PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_gen_exception_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::random_seed_gen_exception PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_random_seed_gen_exception_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::periodic_sampler #############

        set(abseil_absl_periodic_sampler_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_periodic_sampler_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_periodic_sampler_FRAMEWORKS_DEBUG}" "${abseil_absl_periodic_sampler_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_periodic_sampler_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_periodic_sampler_DEPS_TARGET)
            add_library(abseil_absl_periodic_sampler_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_periodic_sampler_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_periodic_sampler_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_periodic_sampler_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_periodic_sampler_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_periodic_sampler_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_periodic_sampler_LIBS_DEBUG}"
                              "${abseil_absl_periodic_sampler_LIB_DIRS_DEBUG}"
                              "${abseil_absl_periodic_sampler_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_periodic_sampler_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_periodic_sampler_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_periodic_sampler_DEPS_TARGET
                              abseil_absl_periodic_sampler_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_periodic_sampler"
                              "${abseil_absl_periodic_sampler_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::periodic_sampler
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_periodic_sampler_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_periodic_sampler_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_periodic_sampler_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::periodic_sampler
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_periodic_sampler_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::periodic_sampler PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_periodic_sampler_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::periodic_sampler PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_periodic_sampler_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::periodic_sampler PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_periodic_sampler_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::periodic_sampler PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_periodic_sampler_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::periodic_sampler PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_periodic_sampler_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::exponential_biased #############

        set(abseil_absl_exponential_biased_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_exponential_biased_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_exponential_biased_FRAMEWORKS_DEBUG}" "${abseil_absl_exponential_biased_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_exponential_biased_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_exponential_biased_DEPS_TARGET)
            add_library(abseil_absl_exponential_biased_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_exponential_biased_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_exponential_biased_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_exponential_biased_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_exponential_biased_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_exponential_biased_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_exponential_biased_LIBS_DEBUG}"
                              "${abseil_absl_exponential_biased_LIB_DIRS_DEBUG}"
                              "${abseil_absl_exponential_biased_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_exponential_biased_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_exponential_biased_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_exponential_biased_DEPS_TARGET
                              abseil_absl_exponential_biased_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_exponential_biased"
                              "${abseil_absl_exponential_biased_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::exponential_biased
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_exponential_biased_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_exponential_biased_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_exponential_biased_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::exponential_biased
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_exponential_biased_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::exponential_biased PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_exponential_biased_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::exponential_biased PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_exponential_biased_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::exponential_biased PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_exponential_biased_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::exponential_biased PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_exponential_biased_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::exponential_biased PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_exponential_biased_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::numeric_representation #############

        set(abseil_absl_numeric_representation_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_numeric_representation_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_numeric_representation_FRAMEWORKS_DEBUG}" "${abseil_absl_numeric_representation_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_numeric_representation_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_numeric_representation_DEPS_TARGET)
            add_library(abseil_absl_numeric_representation_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_numeric_representation_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_representation_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_representation_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_representation_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_numeric_representation_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_numeric_representation_LIBS_DEBUG}"
                              "${abseil_absl_numeric_representation_LIB_DIRS_DEBUG}"
                              "${abseil_absl_numeric_representation_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_numeric_representation_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_numeric_representation_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_numeric_representation_DEPS_TARGET
                              abseil_absl_numeric_representation_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_numeric_representation"
                              "${abseil_absl_numeric_representation_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::numeric_representation
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_representation_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_representation_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_numeric_representation_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::numeric_representation
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_numeric_representation_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::numeric_representation PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_representation_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::numeric_representation PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_representation_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::numeric_representation PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_representation_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::numeric_representation PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_representation_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::numeric_representation PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_representation_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::numeric #############

        set(abseil_absl_numeric_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_numeric_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_numeric_FRAMEWORKS_DEBUG}" "${abseil_absl_numeric_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_numeric_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_numeric_DEPS_TARGET)
            add_library(abseil_absl_numeric_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_numeric_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_numeric_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_numeric_LIBS_DEBUG}"
                              "${abseil_absl_numeric_LIB_DIRS_DEBUG}"
                              "${abseil_absl_numeric_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_numeric_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_numeric_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_numeric_DEPS_TARGET
                              abseil_absl_numeric_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_numeric"
                              "${abseil_absl_numeric_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::numeric
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_numeric_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::numeric
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_numeric_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::numeric PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::numeric PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::numeric PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::numeric PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::numeric PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_numeric_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::int128 #############

        set(abseil_absl_int128_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_int128_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_int128_FRAMEWORKS_DEBUG}" "${abseil_absl_int128_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_int128_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_int128_DEPS_TARGET)
            add_library(abseil_absl_int128_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_int128_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_int128_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_int128_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_int128_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_int128_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_int128_LIBS_DEBUG}"
                              "${abseil_absl_int128_LIB_DIRS_DEBUG}"
                              "${abseil_absl_int128_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_int128_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_int128_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_int128_DEPS_TARGET
                              abseil_absl_int128_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_int128"
                              "${abseil_absl_int128_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::int128
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_int128_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_int128_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_int128_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::int128
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_int128_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::int128 PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_int128_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::int128 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_int128_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::int128 PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_int128_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::int128 PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_int128_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::int128 PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_int128_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::bits #############

        set(abseil_absl_bits_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_bits_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_bits_FRAMEWORKS_DEBUG}" "${abseil_absl_bits_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_bits_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_bits_DEPS_TARGET)
            add_library(abseil_absl_bits_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_bits_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bits_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bits_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bits_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_bits_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_bits_LIBS_DEBUG}"
                              "${abseil_absl_bits_LIB_DIRS_DEBUG}"
                              "${abseil_absl_bits_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_bits_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_bits_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_bits_DEPS_TARGET
                              abseil_absl_bits_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_bits"
                              "${abseil_absl_bits_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::bits
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_bits_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_bits_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_bits_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::bits
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_bits_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::bits PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bits_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::bits PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bits_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bits PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_bits_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::bits PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bits_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::bits PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_bits_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::meta #############

        set(abseil_absl_meta_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_meta_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_meta_FRAMEWORKS_DEBUG}" "${abseil_absl_meta_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_meta_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_meta_DEPS_TARGET)
            add_library(abseil_absl_meta_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_meta_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_meta_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_meta_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_meta_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_meta_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_meta_LIBS_DEBUG}"
                              "${abseil_absl_meta_LIB_DIRS_DEBUG}"
                              "${abseil_absl_meta_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_meta_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_meta_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_meta_DEPS_TARGET
                              abseil_absl_meta_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_meta"
                              "${abseil_absl_meta_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::meta
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_meta_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_meta_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_meta_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::meta
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_meta_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::meta PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_meta_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::meta PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_meta_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::meta PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_meta_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::meta PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_meta_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::meta PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_meta_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::type_traits #############

        set(abseil_absl_type_traits_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_type_traits_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_type_traits_FRAMEWORKS_DEBUG}" "${abseil_absl_type_traits_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_type_traits_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_type_traits_DEPS_TARGET)
            add_library(abseil_absl_type_traits_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_type_traits_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_type_traits_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_type_traits_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_type_traits_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_type_traits_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_type_traits_LIBS_DEBUG}"
                              "${abseil_absl_type_traits_LIB_DIRS_DEBUG}"
                              "${abseil_absl_type_traits_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_type_traits_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_type_traits_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_type_traits_DEPS_TARGET
                              abseil_absl_type_traits_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_type_traits"
                              "${abseil_absl_type_traits_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::type_traits
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_type_traits_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_type_traits_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_type_traits_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::type_traits
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_type_traits_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::type_traits PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_type_traits_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::type_traits PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_type_traits_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::type_traits PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_type_traits_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::type_traits PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_type_traits_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::type_traits PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_type_traits_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::flags_commandlineflag_internal #############

        set(abseil_absl_flags_commandlineflag_internal_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_flags_commandlineflag_internal_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_flags_commandlineflag_internal_FRAMEWORKS_DEBUG}" "${abseil_absl_flags_commandlineflag_internal_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_flags_commandlineflag_internal_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_flags_commandlineflag_internal_DEPS_TARGET)
            add_library(abseil_absl_flags_commandlineflag_internal_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_flags_commandlineflag_internal_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_internal_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_internal_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_internal_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_flags_commandlineflag_internal_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_flags_commandlineflag_internal_LIBS_DEBUG}"
                              "${abseil_absl_flags_commandlineflag_internal_LIB_DIRS_DEBUG}"
                              "${abseil_absl_flags_commandlineflag_internal_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_flags_commandlineflag_internal_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_flags_commandlineflag_internal_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_flags_commandlineflag_internal_DEPS_TARGET
                              abseil_absl_flags_commandlineflag_internal_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_flags_commandlineflag_internal"
                              "${abseil_absl_flags_commandlineflag_internal_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::flags_commandlineflag_internal
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_internal_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_internal_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_flags_commandlineflag_internal_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::flags_commandlineflag_internal
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_flags_commandlineflag_internal_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::flags_commandlineflag_internal PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_internal_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_commandlineflag_internal PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_internal_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_commandlineflag_internal PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_internal_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_commandlineflag_internal PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_internal_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::flags_commandlineflag_internal PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_flags_commandlineflag_internal_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::leak_check #############

        set(abseil_absl_leak_check_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_leak_check_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_leak_check_FRAMEWORKS_DEBUG}" "${abseil_absl_leak_check_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_leak_check_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_leak_check_DEPS_TARGET)
            add_library(abseil_absl_leak_check_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_leak_check_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_leak_check_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_leak_check_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_leak_check_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_leak_check_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_leak_check_LIBS_DEBUG}"
                              "${abseil_absl_leak_check_LIB_DIRS_DEBUG}"
                              "${abseil_absl_leak_check_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_leak_check_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_leak_check_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_leak_check_DEPS_TARGET
                              abseil_absl_leak_check_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_leak_check"
                              "${abseil_absl_leak_check_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::leak_check
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_leak_check_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_leak_check_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_leak_check_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::leak_check
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_leak_check_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::leak_check PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_leak_check_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::leak_check PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_leak_check_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::leak_check PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_leak_check_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::leak_check PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_leak_check_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::leak_check PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_leak_check_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::node_slot_policy #############

        set(abseil_absl_node_slot_policy_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_node_slot_policy_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_node_slot_policy_FRAMEWORKS_DEBUG}" "${abseil_absl_node_slot_policy_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_node_slot_policy_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_node_slot_policy_DEPS_TARGET)
            add_library(abseil_absl_node_slot_policy_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_node_slot_policy_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_slot_policy_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_node_slot_policy_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_node_slot_policy_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_node_slot_policy_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_node_slot_policy_LIBS_DEBUG}"
                              "${abseil_absl_node_slot_policy_LIB_DIRS_DEBUG}"
                              "${abseil_absl_node_slot_policy_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_node_slot_policy_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_node_slot_policy_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_node_slot_policy_DEPS_TARGET
                              abseil_absl_node_slot_policy_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_node_slot_policy"
                              "${abseil_absl_node_slot_policy_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::node_slot_policy
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_slot_policy_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_node_slot_policy_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_node_slot_policy_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::node_slot_policy
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_node_slot_policy_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::node_slot_policy PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_node_slot_policy_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::node_slot_policy PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_slot_policy_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::node_slot_policy PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_node_slot_policy_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::node_slot_policy PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_node_slot_policy_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::node_slot_policy PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_node_slot_policy_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::hashtable_debug_hooks #############

        set(abseil_absl_hashtable_debug_hooks_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_hashtable_debug_hooks_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_hashtable_debug_hooks_FRAMEWORKS_DEBUG}" "${abseil_absl_hashtable_debug_hooks_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_hashtable_debug_hooks_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_hashtable_debug_hooks_DEPS_TARGET)
            add_library(abseil_absl_hashtable_debug_hooks_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_hashtable_debug_hooks_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_hooks_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_hooks_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_hooks_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_hashtable_debug_hooks_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_hashtable_debug_hooks_LIBS_DEBUG}"
                              "${abseil_absl_hashtable_debug_hooks_LIB_DIRS_DEBUG}"
                              "${abseil_absl_hashtable_debug_hooks_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_hashtable_debug_hooks_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_hashtable_debug_hooks_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_hashtable_debug_hooks_DEPS_TARGET
                              abseil_absl_hashtable_debug_hooks_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_hashtable_debug_hooks"
                              "${abseil_absl_hashtable_debug_hooks_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::hashtable_debug_hooks
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_hooks_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_hooks_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_hashtable_debug_hooks_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::hashtable_debug_hooks
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_hashtable_debug_hooks_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::hashtable_debug_hooks PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_hooks_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtable_debug_hooks PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_hooks_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtable_debug_hooks PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_hooks_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtable_debug_hooks PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_hooks_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::hashtable_debug_hooks PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_hashtable_debug_hooks_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::counting_allocator #############

        set(abseil_absl_counting_allocator_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_counting_allocator_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_counting_allocator_FRAMEWORKS_DEBUG}" "${abseil_absl_counting_allocator_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_counting_allocator_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_counting_allocator_DEPS_TARGET)
            add_library(abseil_absl_counting_allocator_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_counting_allocator_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_counting_allocator_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_counting_allocator_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_counting_allocator_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_counting_allocator_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_counting_allocator_LIBS_DEBUG}"
                              "${abseil_absl_counting_allocator_LIB_DIRS_DEBUG}"
                              "${abseil_absl_counting_allocator_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_counting_allocator_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_counting_allocator_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_counting_allocator_DEPS_TARGET
                              abseil_absl_counting_allocator_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_counting_allocator"
                              "${abseil_absl_counting_allocator_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::counting_allocator
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_counting_allocator_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_counting_allocator_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_counting_allocator_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::counting_allocator
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_counting_allocator_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::counting_allocator PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_counting_allocator_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::counting_allocator PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_counting_allocator_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::counting_allocator PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_counting_allocator_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::counting_allocator PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_counting_allocator_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::counting_allocator PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_counting_allocator_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::algorithm #############

        set(abseil_absl_algorithm_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_algorithm_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_algorithm_FRAMEWORKS_DEBUG}" "${abseil_absl_algorithm_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_algorithm_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_algorithm_DEPS_TARGET)
            add_library(abseil_absl_algorithm_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_algorithm_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_algorithm_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_algorithm_LIBS_DEBUG}"
                              "${abseil_absl_algorithm_LIB_DIRS_DEBUG}"
                              "${abseil_absl_algorithm_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_algorithm_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_algorithm_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_algorithm_DEPS_TARGET
                              abseil_absl_algorithm_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_algorithm"
                              "${abseil_absl_algorithm_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::algorithm
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_algorithm_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::algorithm
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_algorithm_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::algorithm PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::algorithm PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::algorithm PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::algorithm PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::algorithm PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_algorithm_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::prefetch #############

        set(abseil_absl_prefetch_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_prefetch_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_prefetch_FRAMEWORKS_DEBUG}" "${abseil_absl_prefetch_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_prefetch_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_prefetch_DEPS_TARGET)
            add_library(abseil_absl_prefetch_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_prefetch_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_prefetch_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_prefetch_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_prefetch_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_prefetch_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_prefetch_LIBS_DEBUG}"
                              "${abseil_absl_prefetch_LIB_DIRS_DEBUG}"
                              "${abseil_absl_prefetch_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_prefetch_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_prefetch_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_prefetch_DEPS_TARGET
                              abseil_absl_prefetch_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_prefetch"
                              "${abseil_absl_prefetch_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::prefetch
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_prefetch_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_prefetch_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_prefetch_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::prefetch
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_prefetch_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::prefetch PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_prefetch_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::prefetch PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_prefetch_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::prefetch PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_prefetch_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::prefetch PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_prefetch_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::prefetch PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_prefetch_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::fast_type_id #############

        set(abseil_absl_fast_type_id_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_fast_type_id_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_fast_type_id_FRAMEWORKS_DEBUG}" "${abseil_absl_fast_type_id_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_fast_type_id_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_fast_type_id_DEPS_TARGET)
            add_library(abseil_absl_fast_type_id_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_fast_type_id_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_fast_type_id_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_fast_type_id_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_fast_type_id_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_fast_type_id_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_fast_type_id_LIBS_DEBUG}"
                              "${abseil_absl_fast_type_id_LIB_DIRS_DEBUG}"
                              "${abseil_absl_fast_type_id_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_fast_type_id_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_fast_type_id_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_fast_type_id_DEPS_TARGET
                              abseil_absl_fast_type_id_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_fast_type_id"
                              "${abseil_absl_fast_type_id_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::fast_type_id
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_fast_type_id_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_fast_type_id_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_fast_type_id_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::fast_type_id
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_fast_type_id_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::fast_type_id PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_fast_type_id_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::fast_type_id PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_fast_type_id_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::fast_type_id PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_fast_type_id_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::fast_type_id PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_fast_type_id_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::fast_type_id PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_fast_type_id_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::pretty_function #############

        set(abseil_absl_pretty_function_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_pretty_function_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_pretty_function_FRAMEWORKS_DEBUG}" "${abseil_absl_pretty_function_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_pretty_function_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_pretty_function_DEPS_TARGET)
            add_library(abseil_absl_pretty_function_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_pretty_function_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_pretty_function_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_pretty_function_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_pretty_function_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_pretty_function_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_pretty_function_LIBS_DEBUG}"
                              "${abseil_absl_pretty_function_LIB_DIRS_DEBUG}"
                              "${abseil_absl_pretty_function_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_pretty_function_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_pretty_function_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_pretty_function_DEPS_TARGET
                              abseil_absl_pretty_function_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_pretty_function"
                              "${abseil_absl_pretty_function_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::pretty_function
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_pretty_function_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_pretty_function_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_pretty_function_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::pretty_function
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_pretty_function_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::pretty_function PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_pretty_function_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::pretty_function PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_pretty_function_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::pretty_function PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_pretty_function_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::pretty_function PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_pretty_function_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::pretty_function PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_pretty_function_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::core_headers #############

        set(abseil_absl_core_headers_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_core_headers_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_core_headers_FRAMEWORKS_DEBUG}" "${abseil_absl_core_headers_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_core_headers_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_core_headers_DEPS_TARGET)
            add_library(abseil_absl_core_headers_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_core_headers_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_core_headers_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_core_headers_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_core_headers_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_core_headers_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_core_headers_LIBS_DEBUG}"
                              "${abseil_absl_core_headers_LIB_DIRS_DEBUG}"
                              "${abseil_absl_core_headers_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_core_headers_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_core_headers_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_core_headers_DEPS_TARGET
                              abseil_absl_core_headers_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_core_headers"
                              "${abseil_absl_core_headers_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::core_headers
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_core_headers_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_core_headers_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_core_headers_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::core_headers
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_core_headers_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::core_headers PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_core_headers_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::core_headers PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_core_headers_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::core_headers PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_core_headers_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::core_headers PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_core_headers_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::core_headers PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_core_headers_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::dynamic_annotations #############

        set(abseil_absl_dynamic_annotations_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_dynamic_annotations_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_dynamic_annotations_FRAMEWORKS_DEBUG}" "${abseil_absl_dynamic_annotations_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_dynamic_annotations_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_dynamic_annotations_DEPS_TARGET)
            add_library(abseil_absl_dynamic_annotations_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_dynamic_annotations_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_dynamic_annotations_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_dynamic_annotations_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_dynamic_annotations_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_dynamic_annotations_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_dynamic_annotations_LIBS_DEBUG}"
                              "${abseil_absl_dynamic_annotations_LIB_DIRS_DEBUG}"
                              "${abseil_absl_dynamic_annotations_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_dynamic_annotations_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_dynamic_annotations_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_dynamic_annotations_DEPS_TARGET
                              abseil_absl_dynamic_annotations_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_dynamic_annotations"
                              "${abseil_absl_dynamic_annotations_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::dynamic_annotations
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_dynamic_annotations_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_dynamic_annotations_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_dynamic_annotations_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::dynamic_annotations
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_dynamic_annotations_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::dynamic_annotations PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_dynamic_annotations_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::dynamic_annotations PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_dynamic_annotations_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::dynamic_annotations PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_dynamic_annotations_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::dynamic_annotations PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_dynamic_annotations_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::dynamic_annotations PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_dynamic_annotations_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## COMPONENT absl::config #############

        set(abseil_absl_config_FRAMEWORKS_FOUND_DEBUG "")
        conan_find_apple_frameworks(abseil_absl_config_FRAMEWORKS_FOUND_DEBUG "${abseil_absl_config_FRAMEWORKS_DEBUG}" "${abseil_absl_config_FRAMEWORK_DIRS_DEBUG}")

        set(abseil_absl_config_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET abseil_absl_config_DEPS_TARGET)
            add_library(abseil_absl_config_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET abseil_absl_config_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_config_FRAMEWORKS_FOUND_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_config_SYSTEM_LIBS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_config_DEPENDENCIES_DEBUG}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'abseil_absl_config_DEPS_TARGET' to all of them
        conan_package_library_targets("${abseil_absl_config_LIBS_DEBUG}"
                              "${abseil_absl_config_LIB_DIRS_DEBUG}"
                              "${abseil_absl_config_BIN_DIRS_DEBUG}" # package_bindir
                              "${abseil_absl_config_LIBRARY_TYPE_DEBUG}"
                              "${abseil_absl_config_IS_HOST_WINDOWS_DEBUG}"
                              abseil_absl_config_DEPS_TARGET
                              abseil_absl_config_LIBRARIES_TARGETS
                              "_DEBUG"
                              "abseil_absl_config"
                              "${abseil_absl_config_NO_SONAME_MODE_DEBUG}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET absl::config
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Debug>:${abseil_absl_config_OBJECTS_DEBUG}>
                     $<$<CONFIG:Debug>:${abseil_absl_config_LIBRARIES_TARGETS}>
                     APPEND)

        if("${abseil_absl_config_LIBS_DEBUG}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET absl::config
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         abseil_absl_config_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET absl::config PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_config_LINKER_FLAGS_DEBUG}> APPEND)
        set_property(TARGET absl::config PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_config_INCLUDE_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::config PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Debug>:${abseil_absl_config_LIB_DIRS_DEBUG}> APPEND)
        set_property(TARGET absl::config PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Debug>:${abseil_absl_config_COMPILE_DEFINITIONS_DEBUG}> APPEND)
        set_property(TARGET absl::config PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Debug>:${abseil_absl_config_COMPILE_OPTIONS_DEBUG}> APPEND)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_parse APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_usage APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_usage_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_reflection APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::node_hash_map APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flat_hash_map APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::raw_hash_map APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::node_hash_set APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flat_hash_set APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::statusor APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::status APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_random APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::raw_hash_set APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::hashtablez_sampler APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::hash_function_defaults APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::btree APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cord APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cordz_update_scope APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cordz_sample_token APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cordz_info APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cordz_handle APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cordz_statistics APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_distribution_test_util APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_seed_sequences APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::sample_recorder APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_marshalling APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_config APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_program_name APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::synchronization APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::kernel_timeout_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cord_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::str_format APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_nonsecure_base APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_salted_seed_seq APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_pool_urbg APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_seed_material APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_distributions APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::hash APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_private_handle_accessor APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_commandlineflag APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_path_util APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::failure_signal_handler APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::examine_stack APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::symbolize APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::layout APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::any APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::time APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::graphcycles_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::str_format_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::strings APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_randen_engine APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_mock_helpers APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_bit_gen_ref APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::malloc_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::variant APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::optional APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::bad_any_cast APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::strings_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_randen APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_distribution_caller APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::low_level_hash APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::city APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::bind_front APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::any_invocable APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::debugging APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::demangle_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::stacktrace APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::container_memory APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::inlined_vector APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::inlined_vector_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::fixed_array APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::compressed_tuple APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cleanup APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cleanup_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::endian APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::base APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::spinlock_wait APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::utility APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::bad_variant_access APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::bad_optional_access APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::span APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::bad_any_cast_impl APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cordz_functions APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_randen_hwaes APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_generate_real APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::memory APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::function_ref APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::debugging_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::container_common APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::hashtable_debug APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::hash_policy_traits APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::algorithm_container APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::strerror APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::scoped_set_env APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::throw_delegate APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::base_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::raw_logging_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::log_severity APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::errno_saver APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::atomic_hook APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::compare APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::time_zone APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::civil_time APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::cordz_update_tracker APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_uniform_helper APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_randen_hwaes_impl APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_randen_slow APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_platform APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_pcg_engine APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_fastmath APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_wide_multiply APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_iostream_state_saver APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_fast_uniform_bits APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_internal_traits APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::random_seed_gen_exception APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::periodic_sampler APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::exponential_biased APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::numeric_representation APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::numeric APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::int128 APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::bits APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::meta APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::type_traits APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::flags_commandlineflag_internal APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::leak_check APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::node_slot_policy APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::hashtable_debug_hooks APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::counting_allocator APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::algorithm APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::prefetch APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::fast_type_id APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::pretty_function APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::core_headers APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::dynamic_annotations APPEND)
    set_property(TARGET abseil::abseil PROPERTY INTERFACE_LINK_LIBRARIES absl::config APPEND)

########## For the modules (FindXXX)
set(abseil_LIBRARIES_DEBUG abseil::abseil)
