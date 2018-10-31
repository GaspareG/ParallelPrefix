#include <bits/stdc++.h>
#include <omp.h>
#include "clock.hpp"

int op(int a, int b){ return a^b; }


inline int K1(int t, int m){ return (1<<(m-t)); }
inline int K2(int t){ return (1<<t) - 1; }

inline std::array<int, 2> G1(int t, int k)
{
  int l = (k*(1<<t)) - (1<<(t-1)) - 1;
  int r = (k*(1<<t)) - 1;
  return std::make_array<int, 2>{l, r};
}

inline std::array<int, 2> G2(int t, int k, int m)
{
  int l = (k*(1<<(m-t))) - 1;
  int r = (k*(1<<(m-t))) + (1<<(m-t-1)) - 1;
  return std::make_array<int, 2>{l, r};
}

int main()
{

  int m = 5;
  int n = 1<<m;

  std::vector<int> v(n);
  std::vector<int> v_seq(n);
  std::vector<int> v_par(n);

  for(int i=0; i<n; i++) v[i] = i;

  auto start_time_seq = spm::timer::start();
  std::partial_sum(std::begin(v), std::end(v), std::begin(v_seq), op);
  auto stop_time_seq = spm::timer::step(start_time_seq);
  std::cout << "seq time " << stop_time_seq << std::endl;

  // Omp parallel threads
  auto start_time_tot = spm::timer::start();

  // Phase 1
  for(int t=1; t<=m; t++)
  {
    auto start_time = spm::timer::start();
    #pragma omp parallel for schedule(static) num_threads(p)
    for(int k=1; k<=K1(t, m); k++)
    {
      auto [l, r] = G1(t, k);
      v[r] = op(v[l], v[r]);
    }
    auto stop_time = spm::timer::step(start_time);
    std::cout << "\tPHASE " << t << " " << stop_time << std::endl;
  }

  // Phase 2
  for(int t=1; t<m; t++)
  {
    auto start_time = spm::timer::start();
    //#pragma omp parallel for schedule(static)
    for(int k=1; k<=K2(t); k++)
    {
      auto [l, r] = G2(t, k, m);
      v[r] = op(v[l], v[r]);
    }
    auto stop_time = spm::timer::step(start_time);
    std::cout << "\tPHASE " << t << " " << stop_time << std::endl;
  }

  auto stop_time_tot = spm::timer::step(start_time_tot);
  std::cout << "par time " << stop_time_tot << std::endl;

  if( v != v_seq ) std::cout << "ERRORE!" << std::endl;

  return 0;
}


