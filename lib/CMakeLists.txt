cmake_minimum_required(VERSION 3.12)
project(lib)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

file(GLOB MIDI_SRCS midifile/*.cpp)
file(GLOB ANALYSIS_SRCS midi_analysis/*.cpp)

set(SRCS ${MIDI_SRCS} ${ANALYSIS_SRCS} midi_analysis/misc.h midi_analysis/misc.cpp midi_analysis/analysis.h midi_analysis/analysis.cpp include/Analysis.h)

add_library(${PROJECT_NAME} SHARED ${SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${PROJECT_NAME})

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)