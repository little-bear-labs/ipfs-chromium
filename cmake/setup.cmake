set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR};${CMAKE_CURRENT_BINARY_DIR};${CMAKE_MODULE_PATH})
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}")
include(${CMAKE_CXX_COMPILER_ID})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Git)
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
#    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}" "--config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/python.cmake")
if(HAVE_PIP)
    set(CONAN_CMAKE_SILENT_OUTPUT 1)
    execute_process(
        COMMAND conan config home
        OUTPUT_VARIABLE conan_home
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE conan_result
    )
    if(NOT conan_result EQUAL 0)
        execute_process(COMMAND ${Python3_EXECUTABLE} -m pip --quiet install --upgrade conan)
        execute_process(COMMAND conan profile detect)
        execute_process(
            COMMAND conan config home
            OUTPUT_VARIABLE conan_home
            OUTPUT_STRIP_TRAILING_WHITESPACE
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
    if(DEFINED ENV{CONAN_REQUEST_TIMEOUT} AND NOT EXISTS "${conan_home}/global.conf")
        #How incredibly obnoxious is it that conan 2 dropped support for these envs?
        file(WRITE "${conan_home}/global.conf" core.net.http:timeout=$ENV{CONAN_REQUEST_TIMEOUT})
    endif()
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
        file(
            DOWNLOAD
                "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake"
            TLS_VERIFY ON
        )
    endif()
    include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)
    conan_cmake_configure(
        REQUIRES
            abseil/20220623.1
            boost/1.81.0
            gtest/1.13.0
            openssl/1.1.1t
            protobuf/3.21.9
        OPTIONS
            boost/1.81.0:header_only=True
            boost/1.81.0:without_container=False #ipfs_client may use boost
            boost/1.81.0:without_filesystem=False #container uses filesystem
            boost/1.81.0:without_atomic=False #filesystem uses atomic
            boost/1.81.0:without_system=False #filesystem uses system
            boost/1.81.0:without_math=True
            boost/1.81.0:without_wave=True
            boost/1.81.0:without_contract=True
            boost/1.81.0:without_exception=True
            boost/1.81.0:without_graph=True
            boost/1.81.0:without_iostreams=True
            boost/1.81.0:without_locale=True
            boost/1.81.0:without_log=True
            boost/1.81.0:without_program_options=True
            boost/1.81.0:without_random=True
            boost/1.81.0:without_regex=True
            boost/1.81.0:without_mpi=True
            boost/1.81.0:without_serialization=True
            boost/1.81.0:without_coroutine=True
            boost/1.81.0:without_fiber=True
            boost/1.81.0:without_context=True
            boost/1.81.0:without_timer=True
            boost/1.81.0:without_thread=True
            boost/1.81.0:without_chrono=True
            boost/1.81.0:without_date_time=True
            boost/1.81.0:without_graph_parallel=True
            boost/1.81.0:without_stacktrace=True
            boost/1.81.0:without_test=True
            boost/1.81.0:without_type_erasure=True
        GENERATORS
            CMakeDeps
        OUTPUT_QUIET
        PROFILE_AUTO ALL
    )
    conan_cmake_autodetect(settings)
    conan_cmake_install(
        PATH_OR_REFERENCE .
        BUILD missing
        SETTINGS ${settings}
        OUTPUT_QUIET
    )

else()
    message(WARNING "Could not find python 3. Can't use gn or conan. You're on your own for Chromium & dependencies.")
endif()

function(with_vocab target)
    target_include_directories(${target}
        BEFORE
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}"
    )
    find_package(Protobuf REQUIRED)
    target_link_libraries(${target}
        PRIVATE
            protobuf::libprotobuf
    )
    target_include_directories(${target}
        SYSTEM
        BEFORE
        PRIVATE
            ${protobuf_INCLUDE_DIR}
    )
    find_package(absl)
    find_package(Boost)
    #TODO If not available at all... maybe just turn off sha support?
    find_package(OpenSSL
            COMPONENTS
            Crypto
            )
    if(absl_FOUND)
        target_link_libraries(${target}
            PRIVATE # Can't allow these to propagate to component
                absl::statusor
        )
    else()
        message(WARNING "Not pulling vocab/ dependencies for ${target} from Abseil")
    endif()
    if(Boost_FOUND)
        target_link_libraries(${target}
            PRIVATE
                Boost::headers
        )
    else()
        message(WARNING "Not pulling vocab/ dependencies for ${target} from Boost")
    endif()
    if(OpenSSL_FOUND)
        target_link_libraries(${target}
            PRIVATE
                OpenSSL::Crypto
        )
    endif()
endfunction()
