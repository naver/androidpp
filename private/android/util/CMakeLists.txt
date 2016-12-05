set(UTIL_SOURCES
)

set(UTIL_HEADERS
)

if (WIN32)
    list(APPEND UTIL_SOURCES
        win/DisplayMetricsWin.cpp
        win/LogWin.cpp
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

add_library(private.android.util STATIC ${UTIL_HEADERS} ${UTIL_SOURCES})
