/*


*/

#include <bits/stdc++.h>
#include "cxxopts.hpp"
#include "clock.hpp"

using Tin = unsigned long int;

namespace spm
{
  namespace block
  {
    template<class T, T op(T,T)> class parallelPrefixSTL
    {
      private:

        std::vector<T> in;
        unsigned int p;
        bool debug;

      public:

        parallelPrefixSTL(std::vector<T>& nin, unsigned int np)
        {
          in = nin;
          p = np;
          debug = false;
        }

        void setParallelismDegree(unsigned int np)
        {
          p = np;
        }

        void start(std::vector<T>& out)
        {

          // assert(in.size() == out.size())
          auto start_time = spm::timer::start();

          // Building (naerly-)balanced ranges
          int N = static_cast<int>(in.size());
          int block_size = N / p;
          std::vector<std::tuple<int,int>> ranges(p);

          for(unsigned int i=0; i<p; ++i)
          {
            int start = i*block_size;
            int end = (i == p-1) ? N : (i+1)*block_size;
            ranges[i] = std::make_tuple(start, end);
          }

          auto step1 = spm::timer::step(start_time);

          // PHASE 1: for each block compute the prefix-sum

          // Lambda function that given a range (begin, end)
          // compute the prefix sum of only in[begin..end-1]
          auto block_prefix = [&](const std::tuple<int,int>& range)
          {
            int a = std::get<0>(range);
            int b = std::get<1>(range);
      	    out[a] = in[a];
      	    for(++a; a<b; ++a) out[a] = op(in[a],out[a-1]);
          };

          // Spawn threads & join them
          std::vector<std::thread> threads_prefix;
          for(unsigned int i=0; i<p; ++i) threads_prefix.emplace_back(block_prefix, ranges[i]);
          for(auto &t : threads_prefix) t.join();

          auto step2 = spm::timer::step(start_time);

          // PHASE 2: prefix sum of block final elements
          std::vector<T> block_sum(p, 0);
          for(unsigned int i=1; i<p; ++i)
          {
            T el = out[std::get<1>(ranges[i-1])-1];
            block_sum[i] = op(block_sum[i-1], el);
          }

          auto step3 = spm::timer::step(start_time);

          // PHASE 3: parallel range add
          auto block_add = [&](const std::tuple<int,int>& range, T add)
          {
            int a = std::get<0>(range);
            int b = std::get<1>(range);
            for(; a<b; ++a) out[a] = op(out[a], add);
          };

          // Spawn threads & join them
          std::vector<std::thread> threads_sum;
          for(unsigned int i=0; i<p; ++i) threads_sum.emplace_back(block_add, ranges[i], block_sum[i]);
          for(auto &t : threads_sum) t.join();

          auto step4 = spm::timer::step(start_time);

          if(debug)
          {
            step4 = step4-step3;
            step3 = step3-step2;
            step2 = step2-step1;
            std::cerr << "PHASE1 " << step1 << "msec" << std::endl;
            std::cerr << "PHASE2 " << step2 << "msec" << std::endl;
            std::cerr << "PHASE3 " << step3 << "msec" << std::endl;
            std::cerr << "PHASE4 " << step4 << "msec" << std::endl;
          }
        }

        void enableDebug(bool enabled)
        {
          debug = enabled;
        }
    };
  }
}

inline Tin op(Tin a,Tin b){ return a^b; };

#ifndef BENCHMARK
int main()
{

  // Create vector
  std::vector<Tin> v(1<<30);
  std::vector<Tin> v_seq(1<<30);
  std::vector<Tin> v_par(1<<30);

  std::cout << "iota..." << std::endl;
  auto start_time_seq = spm::timer::start();
  std::iota(std::begin(v), std::end(v), 1);
  auto stop_time_seq = spm::timer::step(start_time_seq);
  std::cout << "iota time " << stop_time_seq << std::endl;

  // Build parallel prefix comptator
  spm::block::parallelPrefixSTL<Tin, op> test(v, 1);
  test.enableDebug(true);

  for(int k=30; k>= 20; k--)
  {
    v.resize(1<<k);
    v_seq.resize(1<<k);
    v_par.resize(1<<k);

    // Sequential algorithm
    std::cout << "seq..." << std::endl;
    auto start_time_seq = spm::timer::start();
    std::partial_sum(std::begin(v), std::end(v), std::begin(v_seq), op);
    auto stop_time_seq = spm::timer::step(start_time_seq);
    std::cout << "seq time " << stop_time_seq << std::endl;

    unsigned int max_thread = std::thread::hardware_concurrency();
    for(unsigned int i=1; i<=max_thread; i*=2)
    {
      test.setParallelismDegree(i);
      auto start_time = spm::timer::start();
      test.start(v_par);
      auto stop_time = spm::timer::step(start_time);
      std::cout << "nthreads \t" << i << " time " << stop_time << std::endl;
      if(v_seq != v_par) std::cout << "ERROR" << std::endl;
    }
  }

  return 0;

}
#endif
