find_package (Python3 COMPONENTS Interpreter)
if(Python3_EXECUTABLE)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -m pip --version
        RESULT_VARIABLE pip_result
    )
    if(pip_result EQUAL 0)
        set(HAVE_PIP TRUE)
    else()
        set(HAVE_PIP FALSE)
    endif()
else()
    set(HAVE_PIP FALSE)
    message(WARNING "Can't find python3! This means we can't use depot_tools, and so can't do in-Chromium building of either kind. It also means we can't use conan.")
endif()
