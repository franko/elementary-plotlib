cmake_minimum_required(VERSION 3.1)
project(LibCanvas)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FindPkgConfig)

pkg_search_module(AGG REQUIRED libagg)
pkg_search_module(FREETYPE REQUIRED freetype2)
find_package(Threads REQUIRED)

if (WIN32)
    set(WINDOW_LIB_DIR win32)
else (WIN32)
    set(WINDOW_LIB_DIR xwindow)
endif (WIN32)

add_subdirectory(src)
add_subdirectory(${WINDOW_LIB_DIR})
add_subdirectory(tests)
