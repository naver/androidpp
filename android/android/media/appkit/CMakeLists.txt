set(MEDIA_SOURCES
    AudioFile.cpp
    MediaSurface.cpp
)

set(MEDIA_HEADERS
    AudioFile.h
    MediaSurface.h
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

add_library(android.media.appkit STATIC ${MEDIA_HEADERS} ${MEDIA_SOURCES})
