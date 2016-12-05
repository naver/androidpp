# This file is meant to be read from each project under cmake root.
#  - to refer other project.
#  - to share common variables and macros.

# set ROOT to last added item on'CMAKE_MODULE_PATH'.
list(LENGTH CMAKE_MODULE_PATH _last)
math(EXPR _last "${_last}-1") # get last index of list.
list(GET CMAKE_MODULE_PATH _last ROOT)

# set include(source) directory of ${_project} to ${_target}
macro(INCLUDE_OF _target _project)
    set(${_target} "${ROOT}/${_project}/include")
endmacro()

macro(SOURCE_OF _target _project)
    set(${_target} "${ROOT}/${_project}/src")
endmacro()

# set a list of include(source) files of ${_project} to ${_target}
macro(LIST_HEADER _target _project)
    file(GLOB_RECURSE ${_target} "${ROOT}/${_project}/include/*.h")
endmacro()

macro(LIST_SOURCE _target _project)
    file(GLOB_RECURSE ${_target} "${ROOT}/${_project}/src/*.cpp")
endmacro()

# Projects.
# - value of project variable is directory name of it.
# set(MEDIA "media")
# set(OPENGL "opengl")
# set(OS "os")
# set(UTIL "util")
# set(VIEW "view")

# Include directories.
# include_of(MEDIA_INCLUDES "media")
# include_of(OPENGL_INCLUDES "opengl")
# include_of(OS_INCLUDES "os")
# include_of(UTIL_INCLUDES "util")
# include_of(VIEW_INCLUDES "view")

# set(LIBRARY_INCLUDE_DIR "${LIBRARY_PRODUCT_DIR}/include")
