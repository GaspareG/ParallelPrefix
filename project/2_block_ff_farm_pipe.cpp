#include <vector>
#include <memory>
#include <tuple>
#include <algorithm>
#include <utility>
#include <thread>
#include <numeric>
#include <ff/parallel_for.hpp>
#include "cxxopts.hpp"
#include "clock.hpp"

using Tin = unsigned long int;

template <class T>
  using task_t = std::tuple<int, int, std::optional<T>>;

inline Tin op(Tin a,Tin b){ return a^b; }

inline std::tuple<int,int> getRange(int n, int bs, int i)
{
  int bs = n/p;
  int start = i*bs;
  int end = (i == p-1) ? n : (i+1)*bs;
  return std::make_tuple<int,int>(start, end);
}

#ifndef BENCHMARK
int main()
{

  // Create vector
  std::vector<Tin> v(1<<30, 1);
  std::vector<Tin> v_seq(1<<30);
  std::vector<Tin> v_par(1<<30);

  std::cout << "iota..." << std::endl;
  auto start_time_seq = spm::timer::start();
  std::iota(std::begin(*v), std::end(*v), 1);
  auto stop_time_seq = spm::timer::step(start_time_seq);
  std::cout << "iota time " << stop_time_seq << std::endl;

  // Build parallel prefix comptator
  spm::block::parallelPrefixSTL<Tin, op> test(v, 1);
  test.enableDebug(true);

  // v_par
  std::vector<Tin> block_sum(p);

  ff_Pipe<> pipe(farm_stage1, make_unique<secondStage>(), farm_stage3);

  return 0;

}
#endif
