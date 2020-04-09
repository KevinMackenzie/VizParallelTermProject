cmake_minimum_required(VERSION 3.12)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(Qt5_DIR /usr/local/Cellar/qt/5.14.1/lib/cmake/Qt5)

add_subdirectory(lib)
add_subdirectory(ecse)
add_subdirectory(gui)
