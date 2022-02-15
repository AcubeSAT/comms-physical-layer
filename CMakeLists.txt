cmake_minimum_required(VERSION 3.19)
project(tc_synchronization)

set(CMAKE_CXX_STANDARD 14)

include_directories(inc)

add_executable(tc_synchronization
        inc/filter.h
        inc/fm.h
        inc/gmsk.h
        src/fm.cpp
        src/gmsk.cpp
        src/main.cpp
        src/filter.cpp)
