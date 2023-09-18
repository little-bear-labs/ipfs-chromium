find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_LIST_DIR}/../doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(doxy
        COMMAND ${CMAKE_COMMAND} -E env LC_ALL=C.UTF-8 ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT} > ${CMAKE_CURRENT_BINARY_DIR}/doxygen.out
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        ENVIRONMENT
        VERBATIM
    )
    list(APPEND doc_targets doxy)
else (DOXYGEN_FOUND)
  message(WARNING "Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
