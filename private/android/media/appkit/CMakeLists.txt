set(MEDIA_SOURCES
    AudioFilePrivate.cpp
)

set(MEDIA_HEADERS
    AudioFilePrivate.h
)

if (WIN32)
    list(APPEND MEDIA_SOURCES
        win/AudioFilePrivateMediaFoundationWin.cpp
    )

    list(APPEND MEDIA_HEADERS
        win/AudioFilePrivateMediaFoundationWin.h
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

add_library(private.android.media.appkit STATIC ${MEDIA_HEADERS} ${MEDIA_SOURCES})
