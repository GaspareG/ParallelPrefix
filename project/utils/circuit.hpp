#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP

#include <array>
#include <utility>

namespace spm
{
  namespace circuit
  {

    // First phase
    inline int k1(int t, int m){ return (1<<(m-t)); }

    inline std::array<int, 2> g1(int t, int k)
    {
      int l = (k*(1<<t)) - (1<<(t-1)) - 1;
      int r = (k*(1<<t)) - 1;
      return {l, r};
    }

    // Second phase
    inline int k2(int t){ return (1<<t) - 1; }

    inline std::array<int, 2> g2(int t, int k, int m)
    {
      int l = (k*(1<<(m-t))) - 1;
      int r = (k*(1<<(m-t))) + (1<<(m-t-1)) - 1;
      return {l, r};
    }

  }
}

#endif
