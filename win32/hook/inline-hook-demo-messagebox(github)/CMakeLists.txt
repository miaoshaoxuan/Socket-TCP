cmake_minimum_required(VERSION 3.19)
project(inline_hook_demo)


add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_CXX_STANDARD 14)

add_executable(inline_hook_demo main.cpp)