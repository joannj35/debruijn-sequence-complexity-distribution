cmake_minimum_required(VERSION 3.5.2)
project(debruijn_sequence_complexity)

set(CMAKE_CXX_STANDARD 20)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories(.)
include_directories("Complexity 71")

add_executable(debruijn_sequence_complexity
        "Complexity 71/main.cpp"
        "Complexity 71/SequenceGenerator.cpp"
        "Complexity 71/SequenceGenerator.h"
        "Complexity 71/ComplexityToDebruijn.h" "Complexity 71/ComplexityToDebruijn.cpp" "Complexity 71/PairCombinations.cpp")
