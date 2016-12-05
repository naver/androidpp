set(VIEW_SOURCES
)

set(VIEW_HEADERS
)

if (WIN32)
    list(APPEND VIEW_SOURCES
        win/CursorWin.cpp
    )

    list(APPEND VIEW_HEADERS
        win/CursorWin.h
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

add_library(private.android.view.appkit STATIC ${VIEW_HEADERS} ${VIEW_SOURCES})
