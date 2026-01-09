##################### ROOT CMAKE ###################
cmake_minimum_required(VERSION 3.20)
project(quadratic_exponential LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(qe main.cpp)
target_compile_options(qe PRIVATE -O3 -Wall -Wextra -Wpedantic)
