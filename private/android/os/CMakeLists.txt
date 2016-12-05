set(OS_SOURCES
    BundlePrivateJSON.cpp
    MessageTarget.cpp
)

set(OS_HEADERS
    BundlePrivate.h
    HandlerProvider.h
    MessageTarget.h
)

if (WIN32)
    list(APPEND OS_SOURCES
        win/HandlerProviderWin.cpp
        win/LooperWin.cpp
        win/MessageTargetWin.cpp
    )

    list(APPEND OS_HEADERS
        win/HandlerProviderWin.h
        win/MessageCopyData.h
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

add_subdirectory(appkit)

add_library(private.android.os STATIC ${OS_HEADERS} ${OS_SOURCES})
