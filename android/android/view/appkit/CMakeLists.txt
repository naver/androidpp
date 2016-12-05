set(VIEW_SOURCES
    ContentFrame.cpp
    ContentView.cpp
    EditableContextInputConnection.cpp
    MotionDetector.cpp
    RenderContextList.cpp
)

set(VIEW_HEADERS
    CompositionClause.h
    ContentFrame.h
    ContentView.h
    Cursor.h
    EditableContext.h
    EditableContextInputConnection.h
    MotionDetector.h
    RenderContext.h
    RenderContextList.h
    UIEvent.h
    UIKeyEvent.h
    UIMouseEvent.h
    UIWheelEvent.h
    ViewContext.h
    VirtualKeyMap.h
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

add_library(android.view.appkit STATIC ${VIEW_HEADERS} ${VIEW_SOURCES})
