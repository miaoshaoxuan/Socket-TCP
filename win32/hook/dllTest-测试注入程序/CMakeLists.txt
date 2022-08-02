cmake_minimum_required(VERSION 3.19)
project(dllTest)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_CXX_STANDARD 14)

add_executable(dllTest main.cpp)