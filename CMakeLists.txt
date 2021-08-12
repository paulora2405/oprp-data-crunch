cmake_minimum_required(VERSION 3.3)
project(DataCrunch)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -fopenmp")

add_subdirectory(src)
