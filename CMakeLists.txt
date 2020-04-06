cmake_minimum_required(VERSION 3.12)
project(VizParallelTermProject)

set(CMAKE_CXX_STANDARD 14)

set(PROJECT_SOURCE_DIR src)

include_directories(lib/include)

set(SRCS
        ${PROJECT_SOURCE_DIR}/main.cpp
        ${PROJECT_SOURCE_DIR}/midifile/Binasc.cpp
        ${PROJECT_SOURCE_DIR}/midifile/MidiEvent.cpp
        ${PROJECT_SOURCE_DIR}/midifile/MidiEventList.cpp
        ${PROJECT_SOURCE_DIR}/midifile/MidiFile.cpp
        ${PROJECT_SOURCE_DIR}/midifile/Options.cpp
        ${PROJECT_SOURCE_DIR}/midifile/MidiMessage.cpp)

add_executable(VizParallelTermProject ${SRCS})