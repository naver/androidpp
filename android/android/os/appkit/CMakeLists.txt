set(OS_SOURCES
    MessageChannel.cpp
    MessageHost.cpp
    Process.cpp
    ProcessLauncher.cpp
    Thread.cpp
)

set(OS_HEADERS
    MessageChannel.h
    MessageChannelMessages.h
    MessageHost.h
    MessageHostMessages.h
    Process.h
    ProcessLauncher.h
    ProcessMessages.h
    MessageFilter.h
    Messages.h
    Thread.h
)

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

add_library(android.os.appkit STATIC ${OS_HEADERS} ${OS_SOURCES})
