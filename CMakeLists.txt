cmake_minimum_required(VERSION 3.20)
project(DA_Register_Allocation)

set(CMAKE_CXX_STANDARD 17)

include_directories(code/include)

add_executable(DA_Register_Allocation code/src/main.cpp code/src/parser.cpp)