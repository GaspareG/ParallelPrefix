#!/bin/bash

echo "compile"
g++ --std=c++17 -O3 -lpthread  -o block block.cpp

#g++ --std=c++17 -O3 -lpthread -DNTHREADS=8 -fopenmp -o block_omp block_omp.cpp

echo "STL Block"
time ./block

#echo "OMP Block"
#time ./block_omp
