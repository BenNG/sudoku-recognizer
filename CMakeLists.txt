set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")

cmake_minimum_required(VERSION 3.5.1)
project( sudokuRecognizer )

add_subdirectory (src)

enable_testing()
add_subdirectory (test)