cmake_minimum_required(VERSION 3.18.4)
project(Rose2)

set(CMAKE_CXX_STANDARD 20)

include_directories(include src/util)

add_library(Rose2 SHARED src/Rose.cpp src/Gadget.cpp)

add_executable(Test test/test.cpp)
