set(VIEW_SOURCES
    View.cpp
    ViewGroup.cpp
)

set(VIEW_HEADERS
    InputDevice.h
    InputEvent.h
    KeyEvent.h
    Menu.h
    MenuItem.h
    MotionEvent.h
    View.h
    ViewGroup.h
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
add_subdirectory(inputmethod)

add_library(android.view STATIC ${VIEW_HEADERS} ${VIEW_SOURCES})
