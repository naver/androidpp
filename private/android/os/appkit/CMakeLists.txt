set(OS_SOURCES
)

set(OS_HEADERS
)

if (WIN32)
    list(APPEND OS_SOURCES
        win/MainWin.cpp
        win/ProcessLauncherWin.cpp
        win/ProcessWin.cpp
        win/ThreadWin.cpp
    )

    list(APPEND OS_HEADERS
    )
endif ()

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${LIBRARY_PRODUCT_DIR}/include"
    "${LIBRARY_PRODUCT_DIR}/include/android"
    "${LIBRARY_PRODUCT_DIR}/include/platforms"
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/android"
    "${CMAKE_SOURCE_DIR}/private"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_BINARY_DIR}"
)

add_library(private.android.os.appkit STATIC ${OS_HEADERS} ${OS_SOURCES})
