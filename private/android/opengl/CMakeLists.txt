set(OPENGL_SOURCES
    GLES2/esShader.cpp
    GLES2/esShapes.cpp
    GLES2/esTransform.cpp
    GLES2/esUtil.cpp
)

set(OPENGL_HEADERS
    GLES2/esUtil.h
)

if (WIN32)
    list(APPEND OPENGL_SOURCES
        GLES2/Win32/esUtil_TGA.cpp
        GLES2/Win32/esUtil_win32.cpp
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

add_subdirectory(appkit)

add_library(private.android.opengl STATIC ${OPENGL_HEADERS} ${OPENGL_SOURCES})
