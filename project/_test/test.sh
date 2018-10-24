#!/bin/bash

g++ --std=c++17 -O3 -lpthread -DNTHREADS=8 -o block block.cpp
g++ --std=c++17 -O3 -lpthread -DNTHREADS=8 -fopenmp -o block_omp block_omp.cpp

echo "STL Block"
time ./block

echo "OMP Block"
time ./block_omp
