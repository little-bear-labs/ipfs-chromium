find_program(SHELLCHECK shellcheck)

if(SHELLCHECK)
  file(GLOB_RECURSE
    shell_scripts
    *.sh *.bash
  )
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/shellcheck.out"
    DEPENDS "${shell_scripts}"
    COMMAND "${SHELLCHECK}" --format=gcc ${shell_scripts} > "${CMAKE_CURRENT_BINARY_DIR}/shellcheck.out" || "${CMAKE_COMMAND}" -E true
    COMMENT "Linting bash"
  )
  add_custom_target(static_sh
    ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/shellcheck.out"
    COMMAND "${CMAKE_COMMAND}" -E cat "${CMAKE_CURRENT_BINARY_DIR}/shellcheck.out"
  )
endif()
