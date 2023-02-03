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
    set(CONAN_CMAKE_SILENT_OUTPUT 1)
    execute_process(COMMAND ${Python3_EXECUTABLE} -m pip --quiet install --upgrade conan)
    if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/conan.cmake")
        file(
            DOWNLOAD
                "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_CURRENT_LIST_DIR}/conan.cmake"
            TLS_VERIFY ON
        )
    endif()
    include(${CMAKE_CURRENT_LIST_DIR}/conan.cmake)
    conan_cmake_configure(
        REQUIRES
            abseil/20220623.1
        GENERATORS
            cmake_find_package
        OUTPUT_QUIET
    )
    conan_cmake_autodetect(settings)
    conan_cmake_install(
        PATH_OR_REFERENCE .
        BUILD missing
        SETTINGS ${settings}
        OUTPUT_QUIET
    )

    if("${DEPOT_TOOLS_DIRECTORY}" STREQUAL "DETECT_FROM_PATH")
        find_program(
            tool_path
            NAMES autoninja.py autoninja autoninja.bat gn.py gn gn.bat #Not all of these may be marked as executable, which is OK but makes them unfindable.
            NO_CACHE
        )
        if(tool_path)
            get_filename_component(
                DEPOT_TOOLS_DIRECTORY
                "${tool_path}"
                DIRECTORY
            )
        endif()
    endif()
    if(EXISTS "${DEPOT_TOOLS_DIRECTORY}/ninja.py")
        message(STATUS "depot_tools found at ${DEPOT_TOOLS_DIRECTORY}")
        set(DEPOT_TOOLS_NINJA_PY "${DEPOT_TOOLS_DIRECTORY}/ninja.py")
    else()
        message(WARNING "DEPOT_TOOLS_DIRECTORY(${DEPOT_TOOLS_DIRECTORY}) not properly leading to a ninja.py.py - will not be doing a proper build of the component inside the Chromium tree.")
    endif()
else()
    message(WARNING "Could not find python 3. Can't use gn or conan. You're on your own for Chromium & dependencies.")
endif()
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/library/base/debug")
file(TOUCH          "${CMAKE_CURRENT_BINARY_DIR}/library/base/debug/debugging_buildflags.h")
