#!/bin/bash

mkdir -p bin
rm "bin/benchmark"

echo "Compiling..."
export FF_ROOT=$(pwd)
time g++ -O3 -Ofast -Wall -Wextra -pedantic -std=c++17 -fconcepts -lpthread -fopenmp -march=native -I$FF_ROOT -o ./bin/benchmark benchmark.cpp

code=$?

echo ""
echo "Done! (return code ${code})"
echo "Executable in ./bin/benchmark"

echo ""
echo "Printing help..."

echo ""
./bin/benchmark --help

#  -fcilkplus -lcilkrts
