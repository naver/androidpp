set(OPENGL_SOURCES
)

set(OPENGL_HEADERS
)

if (WIN32)
    list(APPEND OPENGL_SOURCES
        win/EGLPbufferSurfaceANGLEWin.cpp
    )

    list(APPEND OPENGL_HEADERS
    )
endif ()

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

add_library(private.android.opengl.appkit STATIC ${OPENGL_HEADERS} ${OPENGL_SOURCES})
