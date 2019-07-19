cmake_minimum_required(VERSION 2.8)
project(JISHO)
set(JISHO_VERSION_MAJOR 1)
set(JISHO_VERSION_MINOR 0)

set(BASE_CXX_FLAGS
    "-std=gnu++17 -pthread")

set(CMAKE_CXX_FLAGS
    "${BASE_CXX_FLAGS} -O0 -g -fmax-errors=5")

set(CMAKE_CXX_FLAGS_RELEASE
    "${BASE_CXX_FLAGS} -O3")

include_directories(include)
add_subdirectory(src)
