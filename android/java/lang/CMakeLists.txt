set(LANG_SOURCES
)

set(LANG_HEADERS
    ../lang.h
    CharSequence.h
    Object.h
    StringInlines.h
    System.h
)

if (WIN32)
    list(APPEND LANG_SOURCES
        win/SystemWin.cpp
    )
endif ()

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${LIBRARY_PRODUCT_DIR}/include"
    "${LIBRARY_PRODUCT_DIR}/include/android"
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/android"
    "${CMAKE_SOURCE_DIR}/private"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_BINARY_DIR}"
)

add_library(java.lang STATIC ${LANG_HEADERS} ${LANG_SOURCES})
