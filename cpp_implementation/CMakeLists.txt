cmake_minimum_required(VERSION 3.18)

SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
SET(CMAKE_CXX_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER arm-none-eabi-g++)

project(tc_synchronization)

set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/inc" "${PROJECT_SOURCE_DIR}/external/etl/include")
include_directories(inc)
include_directories(inc/FreeRTOSTasks)
include_directories(src)
include_directories(src/FreeRTOSTasks)
include_directories(lib/FreeRTOS)
include_directories(lib/FreeRTOS/Source)
include_directories(lib/FreeRTOS/Source/include)
include_directories(lib/FreeRTOS/Source/portable)
include_directories(lib/FreeRTOS/Source/portable/GCC)
include_directories(lib/FreeRTOS/Source/portable/GCC/SAM)
include_directories(lib/FreeRTOS/Source/portable/GCC/SAM/CM7)
include_directories(lib/FreeRTOS/Source/portable/MemMang)

add_subdirectory(external/etl)

add_library(common OBJECT
        src/BCHDecoder.cpp
        src/ConvolutionalEncoder.cpp
        src/oqpsk.cpp
        src/fm.cpp
        src/gmsk.cpp
        src/filter.cpp
        src/ldpc.cpp)

file(GLOB_RECURSE FREERTOS_SOURCES "lib/FreeRTOS/*.c")

add_executable(tc_synchronization
        $<TARGET_OBJECTS:common>
        src/main.cpp
        src/main.c
        ${FREERTOS_SOURCES}
        )

IF (EXISTS "${PROJECT_SOURCE_DIR}/external/Catch2/CMakeLists.txt")
    # Gather all the .cpp files corresponding to tests
    file(GLOB test_main_SRC "test/*.cpp")
    file(GLOB test_SRC "test/**/*.cpp")

    add_subdirectory(external/Catch2)
    add_executable(tests
            $<TARGET_OBJECTS:common>
            ${test_main_SRC}
            ${test_SRC}
            )
    target_link_libraries(tests Catch2::Catch2WithMain)
ENDIF ()

target_link_options(tc_synchronization PRIVATE "-specs=nosys.specs")