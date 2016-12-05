set(MEDIA_SOURCES
    AudioTrack.cpp
    MediaFormat.cpp
    MediaPlayer.cpp
)

set(MEDIA_HEADERS
    AudioFormat.h
    AudioTrack.h
    MediaFormat.h
    MediaPlayer.h
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

add_subdirectory(appkit)

add_library(android.media STATIC ${MEDIA_HEADERS} ${MEDIA_SOURCES})
