#!/bin/env python3

import subprocess

for i in range(16, 32):

  # SEQ
  file = "prefix1_seq_for"
  k = (2**i)
  t = 1
  input = ("1 " * k + "\n").encode("ascii")
  p = subprocess.run( ["./bin/" + file, str(k)], input=input, stdout=subprocess.PIPE, stderr=subprocess.PIPE ) # PIPE)
  print("T", t, "K", k, "E", p.stderr.decode("ascii")[:-1], "["+file+"]" )

  # PAR
  for file in ["prefix2_horn_omp", "prefix3_updownsweep_omp"]:
    for t in [1, 2, 4, 8]:
      k = (2**i)
      input = ("1 " * k + "\n").encode("ascii")
      p = subprocess.run( ["./bin/" + file, str(k), str(t)], input=input, stdout=subprocess.PIPE, stderr=subprocess.PIPE ) # PIPE)
      print("T", t, "K", k, "E", p.stderr.decode("ascii")[:-1], "["+file+"]" )

  print()
