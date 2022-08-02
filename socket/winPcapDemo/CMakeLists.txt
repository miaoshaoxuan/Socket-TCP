cmake_minimum_required(VERSION 3.19)
project(winPcapDemo)

set(CMAKE_CXX_STANDARD 14)

set(INC_DIR ./lib/WpdPack/Include)
set(LINK_DIR ./lib/WpdPack/Lib/)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(Packet wpcap)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(winPcapDemo main.cpp examples.cpp examples.h)

target_link_libraries(winPcapDemo Packet wpcap)
