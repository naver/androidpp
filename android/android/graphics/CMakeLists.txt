set(GRAPHICS_SOURCES
    Bitmap.cpp
)

set(GRAPHICS_HEADERS
    Bitmap.h
    Point.h
    PointF.h
    Rect.h
    RectF.h
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

add_library(android.graphics STATIC ${GRAPHICS_HEADERS} ${GRAPHICS_SOURCES})
