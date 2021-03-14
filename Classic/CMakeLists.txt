cmake_minimum_required(VERSION 3.0)

project(my_sketch)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=medium -w") 

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)

add_executable(test main.cpp)

add_executable(delete_test deletion_main.cpp)
