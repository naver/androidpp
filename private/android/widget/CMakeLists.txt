set(WIDGET_SOURCES
)

set(WIDGET_HEADERS
)

if (WIN32)
    list(APPEND WIDGET_SOURCES
        win/PopupMenuWin.cpp
    )

    list(APPEND WIDGET_HEADERS
        win/PopupMenuWin.h
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

add_library(private.android.widget STATIC ${WIDGET_HEADERS} ${WIDGET_SOURCES})
