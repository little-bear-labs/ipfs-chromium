list(
    PREPEND
        CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_BINARY_DIR}
)
list(
    PREPEND
        CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_LIST_DIR}
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}" "--config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy")
endif()
find_package (Python3 COMPONENTS Interpreter)
if(Python3_EXECUTABLE)
    execute_process(COMMAND ${Python3_EXECUTABLE} -m pip install --upgrade conan)
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        file(
            DOWNLOAD
                "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
            TLS_VERIFY ON
        )
    endif()
    include(${CMAKE_BINARY_DIR}/conan.cmake)
    conan_cmake_configure(
        REQUIRES
            abseil/20220623.1
        GENERATORS
            cmake_find_package
    )
    conan_cmake_autodetect(settings)
    conan_cmake_install(
        PATH_OR_REFERENCE .
        BUILD missing
        SETTINGS ${settings}
    )
else()
    message(WARNING "Could not find python 3. Can't use conan. You're on your own for dependencies.")
endif()
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/library/base/debug")
file(TOUCH          "${CMAKE_CURRENT_BINARY_DIR}/library/base/debug/debugging_buildflags.h")
