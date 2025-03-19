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
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/conanfile.txt.in"
        "${CMAKE_CURRENT_BINARY_DIR}/conanfile.txt"
        @ONLY
    )
    execute_process(
        COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_LIST_DIR}/conan_remotes.py"
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
