#!/bin/bash

mkdir -p "bin"

for name in *.cpp; do
  bn=$(basename $name ".cpp")
  echo "$name $bn"
  g++ -O3 --std=c++17 -fopenmp -o bin/$bn $name
done
