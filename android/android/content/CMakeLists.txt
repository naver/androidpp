set(CONTENT_SOURCES
    Context.cpp
    ContextWrapper.cpp
)

set(CONTENT_HEADERS
    Context.h
    ContextWrapper.h
)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${LIBRARY_PRODUCT_DIR}/include"
    "${LIBRARY_PRODUCT_DIR}/include/android"
    "${LIBRARY_PRODUCT_DIR}/include/ANGLE/include"
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/android"
    "${CMAKE_SOURCE_DIR}/private"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_BINARY_DIR}"
)

add_subdirectory(res)

add_library(android.content STATIC ${CONTENT_HEADERS} ${CONTENT_SOURCES})
