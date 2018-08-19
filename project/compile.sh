#!/bin/bash

for name in *.cpp; do
  bn=$(basename $name ".cpp")
  echo "$name $bn"
  g++ -O2 --std=c++17 -fopenmp -o $bn $name
done
