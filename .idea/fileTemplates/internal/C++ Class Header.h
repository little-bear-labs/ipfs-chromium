#parse("C File Header.h")
#[[#ifndef]]# ${INCLUDE_GUARD}
#[[#define]]# ${INCLUDE_GUARD}

${NAMESPACES_OPEN}

class ${NAME} {
  public:
    ${NAME}();
    ~{NAME}() noexcept;
};

${NAMESPACES_CLOSE}

#[[#endif]]# //${INCLUDE_GUARD}
