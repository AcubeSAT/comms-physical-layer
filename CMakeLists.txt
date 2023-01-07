cmake_minimum_required(VERSION 3.18)
project(physical_layer)

set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/inc" "${PROJECT_SOURCE_DIR}/lib/etl/include")
include_directories(inc)

add_subdirectory(lib/etl)

# No dynamic allocations in the stack
add_compile_options(-Wvla -Walloca)
# We do not use exceptions, nor runtime polymorphism
add_compile_options(-fno-exceptions -fno-rtti)

add_library(common OBJECT
        src/BCHDecoder.cpp
        src/ConvolutionalEncoder.cpp
        src/OQPSKTranscoder.cpp
        src/FMTranscoder.cpp
        src/GMSKTranscoder.cpp
        src/filter.cpp
        src/LDPCEncoder.cpp
        src/Synchronizer.cpp
        src/Channel.cpp )

file(GLOB_RECURSE x86_main_SRC "src/Platform/x86/*.cpp")

add_executable(physical_layer
        $<TARGET_OBJECTS:common>
        ${x86_main_SRC}
        src/main.cpp
        )

IF (EXISTS "${PROJECT_SOURCE_DIR}/lib/Catch2/CMakeLists.txt")
    # Gather all the .cpp files corresponding to tests
    file(GLOB test_main_SRC "test/*.cpp")
    file(GLOB test_SRC "test/**/*.cpp")

    add_subdirectory(lib/Catch2)
    add_executable(tests
            $<TARGET_OBJECTS:common>
            ${test_main_SRC}
            ${x86_main_SRC}
            ${test_SRC}
            )
    target_link_libraries(tests Catch2::Catch2WithMain)
ENDIF ()
