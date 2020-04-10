cmake_minimum_required(VERSION 3.5)

project(testqt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(${lib_INCLUDE_DIRS})

add_executable(testqt
        main.cpp
        mainwindow.cpp
        mainwindow.h
        MidiNoteGraphicsItem.cpp
        MidiNoteGraphicsItem.h
        NoteInfo.cpp NoteInfo.h PianoRollScene.cpp PianoRollScene.h)

target_link_libraries(testqt PRIVATE Qt5::Widgets lib)
