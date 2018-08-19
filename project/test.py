#!/bin/env python3

import subprocess

for i in range(0, 30):
  for t in [1, 2, 4, 8]:
    k = (2**i)
    input = ("1 " * k + "\n").encode("ascii")
    p = subprocess.run( ['./prefix3_par_horn_omp', str(k), str(t)], input=input, stdout=subprocess.PIPE, stderr=subprocess.PIPE ) # PIPE)
    print("T", t, "K", k, "E", p.stderr.decode("ascii")[:-1] )
  print()
