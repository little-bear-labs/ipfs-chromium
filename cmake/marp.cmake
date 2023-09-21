find_program(MARP_EXE NAMES "marp")
if(MARP_EXE)
  set(out "${CMAKE_BINARY_DIR}/doc/slides")
  file(MAKE_DIRECTORY "${out}")
  add_custom_target(gen_slides
    COMMENT "Generated slides from doc/slides/*.md"
    DEPENDS ${doc_targets}
  )
  file(GLOB
      slide_mds
    CONFIGURE_DEPENDS
      doc/slides/*.md
  )
  file(GLOB
      slide_png
    CONFIGURE_DEPENDS
      doc/slides/*.png
  )
  foreach(slide_md ${slide_mds})
    get_filename_component(slide "${slide_md}" NAME_WE)
    add_custom_target(${slide}
      SOURCES "${slide_md}"
      COMMAND "${MARP_EXE}" --pptx --allow-local-files --html=true "${slide_md}" --output "${out}/${slide}.pptx"
      COMMAND "${MARP_EXE}"        --allow-local-files --html=true "${slide_md}" --output "${out}/${slide}.html"
      COMMAND "${CMAKE_COMMAND}" -E copy ${slide_png} "${out}"
    )
    if(DOXYGEN_FOUND AND USE_DOXYGEN)
      add_custom_command(
        TARGET "${slide}"
        POST_BUILD
        COMMAND cmake -E copy "${out}/${slide}.html" "${CMAKE_BINARY_DIR}/doc/html/md_doc_slides_${slide}.html"
        COMMAND cmake -E copy ${out}/* "${CMAKE_BINARY_DIR}/doc/html/"
      )
    endif()
    add_dependencies(gen_slides "${slide}")
  endforeach()
  list(APPEND doc_targets gen_slides)
endif()
