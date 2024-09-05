find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_EXECUTABLE)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -m pip --version
        RESULT_VARIABLE pip_result
        OUTPUT_QUIET
    )
    if(pip_result EQUAL 0)
        set(HAVE_PIP TRUE)
    else()
        set(HAVE_PIP FALSE)
    endif()
    find_program(PYLINT pylint)
else()
    set(HAVE_PIP FALSE)
    message(WARNING "Can't find python3! This means we can't use depot_tools, and so can't do in-Chromium building of either kind. It also means we can't use conan.")
endif()

if(already_included)
  #message(STATUS "python.cmake included more than once. Not an error.")
elseif(PYLINT)
  file(GLOB_RECURSE
    python_sources
    *.py
  )
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/pylint.json"
    DEPENDS "${python_sources}"
    COMMAND "${PYLINT}" --exit-zero --output-format=json2 ${python_sources} > "${CMAKE_CURRENT_BINARY_DIR}/pylint.json"
    COMMENT "Linting python"
  )
  add_custom_target(static_py
    ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pylint.json"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/pylint_thresh.py" "${CMAKE_CURRENT_BINARY_DIR}/pylint.json"
  )
  set(already_included ON)
else()
  message(WARNING "No pylint.")
endif()
