# To build /MDd, uncomment below line.
# set(DEBUG_SUFFIX 1)

add_definitions(-DNOMINMAX -DUNICODE -D_UNICODE -D_WINDOWS -DWINVER=0x601)

if (${MSVC_CXX_ARCHITECTURE_ID} STREQUAL "X86")
    link_directories("${LIBRARY_PRODUCT_DIR}/lib32")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_PRODUCT_DIR}/lib32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_PRODUCT_DIR}/lib32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_PRODUCT_DIR}/bin32)
else ()
    link_directories("${LIBRARY_PRODUCT_DIR}/lib64")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_PRODUCT_DIR}/lib64)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_PRODUCT_DIR}/lib64)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_PRODUCT_DIR}/bin64)
endif ()

add_definitions(
    /wd4018 /wd4068 /wd4099 /wd4100 /wd4127 /wd4138 /wd4146 /wd4180 /wd4189
    /wd4201 /wd4244 /wd4251 /wd4267 /wd4275 /wd4288 /wd4291 /wd4305 /wd4309
    /wd4344 /wd4355 /wd4389 /wd4396 /wd4456 /wd4457 /wd4458 /wd4459 /wd4481
    /wd4503 /wd4505 /wd4510 /wd4512 /wd4530 /wd4610 /wd4611 /wd4702 /wd4706
    /wd4800 /wd4819 /wd4951 /wd4952 /wd4996 /wd6011 /wd6031 /wd6211 /wd6246
    /wd6255 /wd6387
)

# Create pdb files for debugging purposes, also for Release builds
add_compile_options(/Zi /GS)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")

# We do not use exceptions
add_definitions(-D_HAS_EXCEPTIONS=0)
add_compile_options(/EHa- /EHc- /EHs- /fp:except-)

# We have some very large object files that have to be linked
add_compile_options(/analyze- /bigobj)

# Use CRT security features
add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1)

# Turn off certain link features
add_compile_options(/Gy- /openmp- /GF-)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:NOREF /OPT:NOICF")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /OPT:NOREF /OPT:NOICF")

    # To debug linking time issues, uncomment the following three lines:
    #add_compile_options(/Bv)
    #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /VERBOSE /VERBOSE:INCR /TIME")
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /VERBOSE /VERBOSE:INCR /TIME")
elseif (${CMAKE_BUILD_TYPE} MATCHES "Release")
    add_compile_options(/Oy-)
endif ()

if (NOT ${CMAKE_GENERATOR} MATCHES "Ninja")
    link_directories("${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
    add_definitions(/MP)
endif ()
if (NOT ${CMAKE_CXX_FLAGS} STREQUAL "")
    string(REGEX REPLACE "(/EH[a-z]+) " "\\1- " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Disable C++ exceptions
    string(REGEX REPLACE "/EHsc$" "/EHs- /EHc- " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Disable C++ exceptions
    string(REGEX REPLACE "/GR " "/GR- " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Disable RTTI
    string(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Warnings are important
endif ()

foreach (flag_var
    CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    # Use the multithreaded static runtime library instead of the default DLL runtime.
    string(REGEX REPLACE "/MD" "/MD" ${flag_var} "${${flag_var}}")

    # No debug runtime, even in debug builds.
    if (NOT DEBUG_SUFFIX)
        string(REGEX REPLACE "/MDd" "/MD" ${flag_var} "${${flag_var}}")
        string(REGEX REPLACE "/D_DEBUG" "" ${flag_var} "${${flag_var}}")
    endif ()
endforeach ()

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${OUTPUTCONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${OUTPUTCONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${OUTPUTCONFIG}")
endforeach()

macro(ADD_PREFIX_HEADER _target _header)
    get_target_property(_old_compile_flags ${_target} COMPILE_FLAGS)
    if (${_old_compile_flags} STREQUAL "_old_compile_flags-NOTFOUND")
        set(_old_compile_flags "")
    endif ()
    set_target_properties(${_target} PROPERTIES COMPILE_FLAGS "/FI\"${_header}\" ${_old_compile_flags}")
endmacro()

macro(ADD_POST_BUILD_COMMAND _library)
    set(${_library}_POST_BUILD_COMMAND "${CMAKE_BINARY_DIR}/${_library}/postBuild.cmd")
    file(WRITE "${${_library}_POST_BUILD_COMMAND}" "@echo Running ${_library} post build tasks...\n")
    add_custom_command(TARGET ${_library} POST_BUILD COMMAND ${${_library}_POST_BUILD_COMMAND} VERBATIM)
endmacro()

macro(COPY_LIBRARY_HEADERS _library _list _destination)
    file(APPEND "${${_library}_POST_BUILD_COMMAND}" "@mkdir \"${LIBRARY_PRODUCT_DIR}/${_destination}\" >nul 2>nul\n")
    foreach (_file ${_list})
        get_filename_component(_absolute "${_file}" ABSOLUTE)
        file(APPEND "${${_library}_POST_BUILD_COMMAND}" "@xcopy /y /d /f \"${_absolute}\" \"${LIBRARY_PRODUCT_DIR}/${_destination}\" >nul 2>nul\n")
    endforeach ()
endmacro()

macro(COPY_LIBRARY_HEADERS_DIRECTORY _library _source _destination)
    get_filename_component(_absolute "${_source}" ABSOLUTE)
    file(APPEND "${${_library}_POST_BUILD_COMMAND}" "@robocopy /s /e \"${_absolute}/\" \"${LIBRARY_PRODUCT_DIR}/${_destination}/\" \"*.h\" \"*.hpp\" >nul 2>nul\n")
    file(APPEND "${${_library}_POST_BUILD_COMMAND}" "IF %ERRORLEVEL% LEQ 3 set ERRORLEVEL=0\n")
endmacro()

add_definitions(-DWIN32_POSIX -D_WINSOCKAPI_ -D__STDC_FORMAT_MACROS -DHAVE_STRUCT_TIMESPEC)

add_definitions(-DANGLE_WEBKIT_WIN -DGL_GLEXT_PROTOTYPES -DCAIRO_WIN32_STATIC_BUILD -DCURL_STATICLIB -DLIBXML_STATIC -DLIBXSLT_STATIC -DXMD_H)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB")
set(WIN32_SYSTEM_LIBRARIES libcmt msvcrt msvcprt OLDNAMES Version vcruntime ucrt atls Shlwapi winmm)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
if (${MSVC_CXX_ARCHITECTURE_ID} STREQUAL "X86")
    link_directories("${LIBRARY_PRODUCT_DIR}/lib32")
else ()
    link_directories("${LIBRARY_PRODUCT_DIR}/lib64")
endif ()
