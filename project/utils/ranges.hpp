/*
        filename: benchmark.cpp
          author: Gaspare Ferraro <ferraro@gaspa.re>
  student number: 520549
          degree: MD in CS: Data and Knowledge Science and Technologies
            exam: Parallel and Distributed Systems: Paradigms and Models

     description: Define a class for managing ranges

*/

#ifndef RANGES_HPP
#define RANGES_HPP

#include <array>
#include <cmath>
#include <algorithm>

// To avoid threads overhead
// don't create ranges of size less than...
#define MIN_BLOCK_SIZE 1024u

namespace spm
{
  // Class par managing ranges
  class range_t
  {

  private:
    unsigned int parDeg; // parallelism degree
    unsigned int size;   // task size
    unsigned int bsize;  // block size
    unsigned int nblock; // number blocks

  public:
    range_t(unsigned int n, unsigned int p) : size(n), parDeg(p)
    {
      bsize = std::max(MIN_BLOCK_SIZE, n / p);
      nblock = std::ceil(static_cast<double>(size) / static_cast<double>(bsize));
    }

    // get i-th range
    std::array<int, 2> operator()(int i)
    {
      int l = i * bsize;
      int r = (i == nblock - 1) ? size : (i + 1) * bsize;
      return {l, r};
    }

    // Return total number of blocks
    unsigned int blocks()
    {
      return nblock;
    }
  };
}

#endif
