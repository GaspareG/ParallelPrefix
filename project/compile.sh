#!/bin/bash

mkdir -p bin
rm "bin/benchmark"

echo "Compiling..."
g++ -O3 -Ofast -Wall -Wextra -pedantic -std=c++17 -fconcepts -lpthread -fopenmp -marc=native -I$FF_ROOT -o bin/benchmark benchmark.cpp

echo "Done!"
echo "Executable in ./bin/benchmark"
echo ""

echo "printing help..."
echo ""

./bin/benchmark --help

#  -fcilkplus -lcilkrts
