set(VIEW_SOURCES
    ViewHostWindow.cpp
    ViewPrivate.cpp
    WindowProvider.cpp
)

set(VIEW_HEADERS
    KeyEventPrivate.h
    MotionEventPrivate.h
    ViewHostWindow.h
    ViewPrivate.h
    WindowHandle.h
    WindowProvider.h
)

if (WIN32)
    list(APPEND VIEW_SOURCES
        win/MenuWin.cpp
        win/WindowProviderWin.cpp
    )

    list(APPEND VIEW_HEADERS
        win/MenuWin.h
        win/WindowProviderWin.h
    )
endif ()

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${LIBRARY_PRODUCT_DIR}/include"
    "${LIBRARY_PRODUCT_DIR}/include/android"
    "${LIBRARY_PRODUCT_DIR}/include/ANGLE/include"
    "${LIBRARY_PRODUCT_DIR}/include/wtl"
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/android"
    "${CMAKE_SOURCE_DIR}/private"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_BINARY_DIR}"
)

add_subdirectory(appkit)

add_library(private.android.view STATIC ${VIEW_HEADERS} ${VIEW_SOURCES})
