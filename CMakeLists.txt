cmake_minimum_required(VERSION 3.25)
project(debruijn_sequence_complexity)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories("Complexity 71")

add_executable(debruijn_sequence_complexity
        "Complexity 71/main.cpp"
        "Complexity 71/SequenceGenerator.cpp"
        "Complexity 71/SequenceGenerator.h"
        "Complexity 71/ComplexityToDebruijn.h")
