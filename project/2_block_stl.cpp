/*


*/

#include <bits/stdc++.h>
#include "cxxopts.hpp"
#include "clock.hpp"

namespace spm
{
  namespace block
  {
    template<class T> class parallelPrefixSTL
    {
      private:

        std::vector<T> in;
        std::function<T(T,T)> f;
        unsigned int p;
        bool debug;

      public:

        parallelPrefixSTL(std::vector<T>& nin, std::function<T(T,T)> nf, unsigned int np)
        {
          in = nin;
          f = nf;
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
            int end = std::min(N, static_cast<int>((i+1)*block_size));
            ranges[i] = std::make_tuple(start, end);
          }
          std::get<1>(ranges[p-1]) = N;

          auto step1 = spm::timer::step(start_time);

          // PHASE 1: for each block compute the prefix-sum
          std::vector<std::thread> threads_prefix(p);
          auto block_prefix = [&](std::tuple<int,int> range)
          {
            int a = std::get<0>(range);
            int b = std::get<1>(range);
      	    out[a] = in[a];
      	    for(++a; a<b; ++a) out[a] = f(in[a],out[a-1]);
          };

          // Spawn threads
          for(unsigned int i=0; i<p; ++i)
            threads_prefix[i] = std::thread(block_prefix, ranges[i]);

          // Join them
          for(auto &t : threads_prefix)
            t.join();

          auto step2 = spm::timer::step(start_time);

          // PHASE 2: prefix sum of block final elements
          std::vector<T> block_sum(p, 0);
          for(unsigned int i=1; i<p; ++i)
          {
            T el = out[std::get<1>(ranges[i-1])-1];
            block_sum[i] = f(block_sum[i-1], el);
          }

          auto step3 = spm::timer::step(start_time);

          // PHASE 3: parallel range add
          std::vector<std::thread> threads_sum(p);
          auto block_add = [&](std::tuple<int,int> range, int add)
          {
            int a = std::get<0>(range);
            int b = std::get<1>(range);
	          for(; a<b; ++a) out[a] = f(out[a], add);
          };

          for(unsigned int i=0; i<p; ++i) threads_sum[i] = std::thread(block_add, ranges[i], block_sum[i]);
          for(auto &t : threads_sum) t.join();

          auto step4 = spm::timer::step(start_time);

          if(debug)
          {
            step2 = step2-step1;
            step3 = step3-step2;
            step4 = step4-step3;
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


#ifndef BENCHMARK
int main()
{

  auto op = [](int a,int b){return a^b;};
  std::vector<int> v(1<<30);
  std::vector<int> v_seq(1<<30);
  std::vector<int> v_par(1<<30);

  spm::block::parallelPrefixSTL<int> test(v, op, 1);

  test.enableDebug(true);

  std::partial_sum(std::begin(v), std::end(v), std::begin(v_seq), op);

  unsigned int max_thread = std::thread::hardware_concurrency();

  for(unsigned int i=1; i<=max_thread; i*=2)
  {
    test.setParallelismDegree(i);
    auto start_time = spm::timer::start();
    test.start(v_par);
    auto stop_time = spm::timer::step(start_time);
    std::cout << "nthreads \t" << i << " time " << stop_time << std::endl;
    if(v_seq != v_par)
    {
      std::cout << "ERRORE!" << std::endl;
    }
  }

  return 0;
}
#endif
