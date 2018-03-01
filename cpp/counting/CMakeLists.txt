cmake_minimum_required(VERSION 3.5.1)

project(counting)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
set(CMAKE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} \
    -fsanitize=address")

file(GLOB SOURCES "src/*.cpp")

add_executable(counting ${SOURCES})
