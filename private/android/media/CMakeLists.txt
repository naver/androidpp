set(MEDIA_SOURCES
    AudioTrackPrivate.cpp
    MediaPlayerPrivate.cpp
)

set(MEDIA_HEADERS
    AudioTrackPrivate.h
    MediaPlayerPrivate.h
)

if (WIN32)
    list(APPEND MEDIA_SOURCES
        mock/MediaPlayerPrivateMock.cpp

        win/AudioTrackPrivateXAudio2Win.cpp
        win/DataSourceProviderByteStream.cpp
        win/XAudio2AudioDevice.cpp
    )

    list(APPEND MEDIA_HEADERS
        mock/MediaPlayerPrivateMock.h

        win/AudioTrackPrivateXAudio2Win.h
        win/DataSourceProviderByteStream.h
        win/XAudio2AudioDevice.h
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

add_library(private.android.media STATIC ${MEDIA_HEADERS} ${MEDIA_SOURCES})
