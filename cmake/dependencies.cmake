include(python)
list(
    PREPEND
    CMAKE_PREFIX_PATH
    "${CMAKE_CURRENT_BINARY_DIR}/build/${CMAKE_BUILD_TYPE}/generators"
  )
list(
    PREPEND
    CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_BINARY_DIR}/build/${CMAKE_BUILD_TYPE}/generators"
  )
function(dependency name)
  cmake_parse_arguments(
      PARSE_ARGV
      1
      arg
      "" # options
      "VERSION" #single-value
      "" # multi-value
    )
  if(arg_VERSION)
    find_package(
      ${name}
      ${arg_VERSION}
      CONFIG
      QUIET
    )
  else()
    find_package(
      ${name}
      CONFIG
      QUIET
    )
  endif()
  # message(WARNING "${name}_FOUND=${${name}_FOUND}")
  if (${name}_FOUND)
    message(WARNING "${name} found at version ${${name}_VERSION} in ${${name}_DIR} inc ${${protobuf}_INCLUDE_DIR}")
  else()
    message(WARNING "Did not find ${name}, will try conan fetching.")
    if(TESTING)
      set(test_option "True")
    else()
      set(test_option "False")
    endif()
    # message(WARNING "conan install --output-folder \"${CMAKE_CURRENT_BINARY_DIR}\" --options:all "testing=${test_option}" \"${CMAKE_CURRENT_SOURCE_DIR}\"")
    execute_process(
        COMMAND conan install
          --build=missing
          --output-folder "${CMAKE_CURRENT_BINARY_DIR}"
          --options:all "testing=${test_option}"
          -s:a "build_type=${CMAKE_BUILD_TYPE}"
          -s:a "compiler.cppstd=${CXX_VERSION}"
          "${CMAKE_CURRENT_SOURCE_DIR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BUILD_DIR}"
        COMMAND_ERROR_IS_FATAL ANY
      )
    if(arg_VERSION)
      message(WARNING "${arg_VERSION} ${CMAKE_MODULE_PATH}")
      find_package(
        ${name}
        ${arg_VERSION}
        REQUIRED
        CONFIG
      )
    else()
      if(name STREQUAL "protobuf")
        message(FATAL_ERROR "${CMAKE_PREFIX_PATH} ${ARGV1} ${arg_VERSION}")
      endif()
      find_package(
        ${name}
        REQUIRED
        CONFIG
      )
    endif()
  endif()
endfunction()
