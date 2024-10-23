set(SETUP_CMAKE_INCLUDED 1)

set(protobuf_version "3.20.0")
if(USE_DOXYGEN)
    find_program(DOXYGEN_BIN doxygen)
    if (NOT DOXYGEN_BIN)
        set(doxygen_dep "doxygen/1.9.4")
    endif()
else()
    set(doxygen_dep "")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR};${CMAKE_CURRENT_BINARY_DIR};${CMAKE_MODULE_PATH})
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}")
include(${CMAKE_CXX_COMPILER_ID})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(conan_install)

find_package(Git QUIET)
if(USE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
  if(NOT CLANG_TIDY_EXE)
      message(WARNING "clang-tidy not found")
  endif()
endif()

function(with_vocab target)
    target_include_directories(${target}
        BEFORE
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}"
            "${CMAKE_CURRENT_BINARY_DIR}/library"
    )
    find_package(
        Protobuf
        "${protobuf_version}"
        QUIET
        REQUIRED
    )
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
