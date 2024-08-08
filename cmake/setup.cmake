set(SETUP_CMAKE_INCLUDED 1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR};${CMAKE_CURRENT_BINARY_DIR};${CMAKE_MODULE_PATH})
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}")
include(${CMAKE_CXX_COMPILER_ID})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Git QUIET)
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
include("${CMAKE_CURRENT_LIST_DIR}/python.cmake")
if(INSIDE_CONAN)
    message(STATUS "CMake invoked by Conan - assuming the outer Conan did all the dependency work.")
elseif(NOT HAVE_PIP)
    message(WARNING "Could not find python 3. Can't use gn or conan. You're on your own for Chromium & dependencies.")
else()
    set(CONAN_CMAKE_SILENT_OUTPUT 1)
    execute_process(
        COMMAND conan config home
        OUTPUT_VARIABLE conan_home
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE conan_result
    )
    if(NOT conan_result EQUAL 0)
        execute_process(
            COMMAND ${Python3_EXECUTABLE} -m pip --quiet install --upgrade conan
            OUTPUT_QUIET
          )
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
    file(COPY_FILE
      "${CMAKE_CURRENT_LIST_DIR}/conanfile.txt"
      "${CMAKE_CURRENT_BINARY_DIR}/conan.base.txt"
      ONLY_IF_DIFFERENT
      )
    if(USE_DOXYGEN)
        find_package(Doxygen QUIET)
        if (NOT DOXYGEN_FOUND)
            message(WARNING "Will attempt to get doxygen from Conan, which doesn't work on some systems like Github's ubuntu-latest runner.")
            file(APPEND
              "${CMAKE_CURRENT_BINARY_DIR}/conan.base.txt"
              doxygen/1.9.4
            )
        endif()
    endif()
    file(COPY_FILE
      "${CMAKE_CURRENT_BINARY_DIR}/conan.base.txt"
      "${CMAKE_CURRENT_BINARY_DIR}/conanfile.txt"
      ONLY_IF_DIFFERENT
      )
    if("${CMAKE_CURRENT_BINARY_DIR}/conanfile.txt" IS_NEWER_THAN "${CMAKE_CURRENT_BINARY_DIR}/conan.timestamp")
      execute_process(
          COMMAND conan install ${CMAKE_CURRENT_BINARY_DIR} --output-folder=${CMAKE_CURRENT_BINARY_DIR} --build=missing --settings build_type=${CMAKE_BUILD_TYPE} --generator CMakeDeps
          COMMAND "${CMAKE_COMMAND}" -E touch "${CMAKE_CURRENT_BINARY_DIR}/conan.timestamp"
          COMMAND_ERROR_IS_FATAL ANY
          WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
          )
    endif()
endif()

function(with_vocab target)
    target_include_directories(${target}
        BEFORE
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}"
            "${CMAKE_CURRENT_BINARY_DIR}/library"
    )
    find_package(Protobuf QUIET REQUIRED)
    find_package(c-ares QUIET REQUIRED)
    target_link_libraries(${target}
        PRIVATE
            protobuf::libprotobuf
            c-ares::cares
    )
    target_include_directories(${target}
        SYSTEM
        BEFORE
        PRIVATE
            ${protobuf_INCLUDE_DIR}
    )
    find_package(absl QUIET )
    find_package(Boost QUIET REQUIRED CONFIG)
    find_package(OpenSSL
            QUIET
            COMPONENTS
              Crypto
            )
    if(absl_FOUND)
        target_link_libraries(${target}
            PUBLIC
                absl::statusor
        )
    else()
        message(WARNING "Not pulling vocab/ dependencies for ${target} from Abseil")
    endif()
    if(Boost_FOUND)
        target_link_libraries(${target}
            PUBLIC
                Boost::headers
        )
    else()
        message(WARNING "Not pulling vocab/ dependencies for ${target} from Boost")
    endif()
    if(OpenSSL_FOUND)
        target_link_libraries(${target}
            PUBLIC
                OpenSSL::Crypto
                OpenSSL::SSL
        )
    endif()
    find_package(nlohmann_json QUIET)
    if(nlohmann_json_FOUND)
        target_link_libraries(${target}
            PUBLIC
                nlohmann_json::nlohmann_json
        )
    endif()
endfunction()
