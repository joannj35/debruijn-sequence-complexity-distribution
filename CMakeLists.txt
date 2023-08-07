cmake_minimum_required(VERSION 3.5.2)
project(debruijn_sequence_complexity)

set(CMAKE_CXX_STANDARD 20)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fopenmp -fopenmp-targets=nvptx64-nvidia-cuda")
endif()

include_directories(.)
include_directories("Solution")

add_executable(debruijn_sequence_complexity
        "Solution/main.cpp"
        "Solution/SequenceGenerator.cpp"
        "Solution/SequenceGenerator.h"
        "Solution/ComplexityToDebruijn.h" "Solution/ComplexityToDebruijn.cpp" "Solution/PairCombinations.cpp")
