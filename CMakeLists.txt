cmake_minimum_required(VERSION 3.17)
project(DA_Graph_Coloring)

set(CMAKE_CXX_STANDARD 17)

add_executable(myProg
    src/main.cpp
    src/Parser.cpp
    src/Web.cpp
    src/WebBuilder.cpp
    src/InterferenceGraph.cpp
    src/GraphColoring.cpp
    src/OutputWriter.cpp
    src/Menu.cpp
)

target_include_directories(myProg PRIVATE data_structures src)
