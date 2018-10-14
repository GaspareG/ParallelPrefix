#!/bin/env python2.7

from subprocess import Popen, PIPE
import multiprocessing
import math

for i in range(16, 32):

  print "[" + str(2**i) + "]"
  # SEQ
  file = "prefix1_seq_for"
  k = (2**i)
  t = 1
  input = ("1 " * k + "\n").encode("ascii")
  p = Popen(['./bin/prefix2_horn_omp', str(k)], stdin=PIPE, stdout=PIPE, stderr=PIPE)
  output, err = p.communicate(b""+input+"")
  err = err.strip()
  print "T " + str(t) + " K " + str(k) + " E " + str(err) + " [" + file + "]"

  # PAR
  for file in ["prefix2_horn_omp", "prefix3_updownsweep_omp"]:
    print
    for t in range(0, 1+int(math.log(multiprocessing.cpu_count(), 2))):
      t = (2**t)
      k = (2**i)
      input = ("1 " * k + "\n").encode("ascii")
      p = Popen(["./bin/" + file, str(k), str(t)], stdin=PIPE, stdout=PIPE, stderr=PIPE)
      output, err = p.communicate(b""+input+"")
      err = err.strip()
      print "T " + str(t) + " K " + str(k) + " E " + str(err) + " [" + file + "]"

  print
