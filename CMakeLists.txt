cmake_minimum_required(VERSION 3.18)
project(physical_layer)

set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/inc" "${PROJECT_SOURCE_DIR}/lib/etl/include")
include_directories(inc)

add_subdirectory(lib/etl)

add_library(common OBJECT
        src/BCHDecoder.cpp
        src/ConvolutionalEncoder.cpp
        src/OQPSKTranscoder.cpp
        src/FMTranscoder.cpp
        src/GMSKTranscoder.cpp
        src/filter.cpp
        src/LDPCEncoder.cpp
        )

add_executable(physical_layer
        $<TARGET_OBJECTS:common>
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
            ${test_SRC})
    target_link_libraries(tests Catch2::Catch2WithMain)
ENDIF ()
