/*
        filename: circuit.hpp
          author: Gaspare Ferraro <ferraro@gaspa.re>
  student number: 520549
          degree: MD in CS: Data and Knowledge Science and Technologies
            exam: Parallel and Distributed Systems: Paradigms and Models

     description: Functions for creating P(2^k) circuit

*/

#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP

#include <array>
#include <utility>

namespace spm
{
  namespace circuit
  {

    /* First phase */

    // Size of t-th task of 1^ phase
    inline int k1(int t, int m)
    {
      return (1 << (m - t));
    }

    // t = task number | k = iteration number
    inline std::array<int, 2> g1(int t, int k)
    {
      int l = (k * (1 << t)) - (1 << (t - 1)) - 1;
      int r = (k * (1 << t)) - 1;
      return {l, r};
    }

    /* Second phase */

    // Size of t-th task of 2^ phase
    inline int k2(int t)
    {
      return (1 << t) - 1;
    }

    // t = task number | k = iteration number
    inline std::array<int, 2> g2(int t, int k, int m)
    {
      int l = (k * (1 << (m - t))) - 1;
      int r = (k * (1 << (m - t))) + (1 << (m - t - 1)) - 1;
      return {l, r};
    }

  }
}

#endif
