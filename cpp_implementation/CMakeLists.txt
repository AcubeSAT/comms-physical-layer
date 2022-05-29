cmake_minimum_required(VERSION 3.19)
project(tc_synchronization)

set(CMAKE_CXX_STANDARD 14)

include_directories("${PROJECT_SOURCE_DIR}/inc" "${PROJECT_SOURCE_DIR}/external/etl/include")

add_subdirectory(external/etl)

add_library(common OBJECT
        src/oqpsk.cpp
        src/fm.cpp
        src/gmsk.cpp
        src/filter.cpp
        )

add_executable(tc_synchronization
        $<TARGET_OBJECTS:common>
        src/main.cpp
        )

IF (EXISTS "${PROJECT_SOURCE_DIR}/external/Catch2/CMakeLists.txt")
    # Gather all the .cpp files corresponding to tests
    file(GLOB test_main_SRC "tests/*.cpp")
    file(GLOB test_SRC "tests/**/*.cpp")

    add_subdirectory(external/Catch2)
    add_executable(tests
            $<TARGET_OBJECTS:common>
            ${test_main_SRC}
            ${test_SRC}
            )
    target_link_libraries(tests Catch2::Catch2WithMain)
ENDIF ()

