find_package(
    Python
    COMPONENTS Interpreter
    REQUIRED
  )
execute_process(
    COMMAND "${Python_EXECUTABLE}" --version
  )
execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pip --version
    RESULT_VARIABLE PIP_STATUS
  )
if(PIP_STATUS EQUAL 0)
  message(STATUS "Using Python at ${Python_EXECUTABLE}")
else()
  message(WARNING "pip not working!! This will likely cause problems down the line.")
endif()
execute_process(
    COMMAND "conan" --version
    RESULT_VARIABLE CONAN_STATUS
  )
if(CONAN_STATUS EQUAL 0)
  message(STATUS "Have conan.")
else()
  execute_process(
      COMMAND "${Python_EXECUTABLE}" -m pip install --user --upgrade conan
    )
  execute_process(
      COMMAND conan profile detect
      RESULT_VARIABLE CONAN_STATUS
    )
  if(CONAN_STATUS EQUAL 0)
    message(STATUS "Installed conan.")
  else()
    message(WARNING "conan not working!! This will almost certainly cause problems down the line.")
  endif()
endif()
