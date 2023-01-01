cmake_minimum_required(VERSION 3.18.4)
project(Rose2)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -pedantic -Werror -Wconversion -Wno-attributes)

include_directories(include src/util)

add_library(Rose2 SHARED src/Rose.cpp src/Gadget.cpp src/Manager.cpp)

add_executable(Test test/test.cpp)
target_link_libraries(Test Rose2)
