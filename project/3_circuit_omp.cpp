#include <bits/stdc++.h>
#include <omp.h>
#include "clock.hpp"

int op(int a, int b){ return a^b; }


inline int K1(int t, int m){ return (1<<(m-t)); }
inline int K2(int t){ return (1<<t) - 1; }

std::tuple<int,int> G1(int t, int k)
{
  int l = (k*(1<<t)) - (1<<(t-1)) - 1;
  int r = (k*(1<<t)) - 1;
  return std::make_tuple(l, r);
}

std::tuple<int,int> G2(int t, int k, int m)
{
  int l = (k*(1<<(m-t))) - 1;
  int r = (k*(1<<(m-t))) + (1<<(m-t-1)) - 1;
  return std::make_tuple(l, r);
}

int main()
{

  int m = 20;
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
  omp_set_dynamic(0);
  omp_set_num_threads(32);

  auto start_time_tot = spm::timer::start();

  // Phase 1
  for(int t=1; t<=m; t++)
  {
    auto start_time = spm::timer::start();
    #pragma omp parallel for schedule(static)
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
    #pragma omp parallel for schedule(static)
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


