cmake_minimum_required(VERSION 3.12)
project(ecse)

set(CMAKE_CXX_STANDARD 17)

include_directories(${lib_INCLUDE_DIRS})

set(SRCS main.cpp)

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} lib)

