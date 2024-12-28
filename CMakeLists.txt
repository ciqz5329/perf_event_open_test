cmake_minimum_required(VERSION 3.10)
project(perf_event_open_test)

set(CMAKE_C_STANDARD 99)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_CXX_FLAGS_RELEASE "-O0")

set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
add_executable(perf_event_open_test src/main.cpp
)

