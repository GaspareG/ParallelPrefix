#!/bin/bash

g++ --std=c++17 -O3 -lpthread -DNTHREADS=256 -o block block.cpp
./block

