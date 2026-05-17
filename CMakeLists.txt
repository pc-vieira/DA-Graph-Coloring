cmake_minimum_required(VERSION 3.20)
project(DA-GRAPH-COLORING)

set(CMAKE_CXX_STANDARD 17)

include_directories(code/include)

add_executable(DA-GRAPH-COLORING code/src/main.cpp code/src/parser.cpp)