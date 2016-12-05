set(OPENGL_SOURCES
    EGLOffscreenContext.cpp
    EGLPbufferSurface.cpp
    GLExtensions.cpp
    GLTexture.cpp
    GLTextureHandle.cpp
    GLTextureStreamChannel.cpp
    GLTextureStreamHost.cpp
    GLTransportSurface.cpp
)

set(OPENGL_HEADERS
    EGLOffscreenContext.h
    EGLPbufferSurface.h
    GLExtensions.h
    GLTexture.h
    GLTextureHandle.h
    GLTextureStreamChannel.h
    GLTextureStreamChannelMessages.h
    GLTextureStreamHost.h
    GLTextureStreamHostMessages.h
    GLTransportSurface.h
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

add_library(android.opengl.appkit STATIC ${OPENGL_HEADERS} ${OPENGL_SOURCES})
