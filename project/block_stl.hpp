/*
        filename: block_stl.hpp
          author: Gaspare Ferraro <ferraro@gaspa.re>
  student number: 520549
          degree: MD in CS: Data and Knowledge Science and Technologies
            exam: Parallel and Distributed Systems: Paradigms and Models

     description: Implementation with STL Threads of the block parallel prefix computation

     class usage:

      // Create object
      spm::block::parallelPrefixSTL<T, op> pp_stl(input, parDegree);
      // T type of elements in vectors
      // op binary function (T,T) -> T (commutative and associative)
      // input = shared pointer of input vector (of type T)
      // parDegree = number of worker for the computation

      // Change parallelism degree of the computation
      pp_stl.setParallelismDegree(np);
      // np = new parallism degree

      // Start computation of parallel prefix vector
      pp_stl.start(output);
      // output = reference to the output vector (we assume input.size() >= output.size())

      // Get time statistic about last computation
      auto [t1, t2, t3] = pp_stl.getLastTest();
      // t1 = msec time of first phase
      // t2 = msec time of second phase
      // t3 = msec time of third phase
*/

#ifndef BLOCK_STL_HPP
#define BLOCK_STL_HPP

#include <vector>
#include <memory>
#include <array>
#include <algorithm>
#include <utility>
#include <numeric>
#include <thread>
#include "utils/clock.hpp"
#include "utils/ranges.hpp"

namespace spm
{
  namespace block
  {
    template<class T, T op(T,T)> class parallelPrefixSTL
    {

      private:

        std::shared_ptr<std::vector<T>> input;
        std::array<spm::timer::ms_t, 3> last_test = {0, 0, 0};
        unsigned int parDeg;

      public:

        parallelPrefixSTL(std::shared_ptr<std::vector<T>> const in, unsigned int np) : input(in), parDeg(np) {}

        void setParallelismDegree(unsigned int np)
        {
          parDeg = np;
        }

        void start(std::vector<T>& output)
        {

          // assert(in.size() == out.size())

          unsigned int N = static_cast<unsigned int>(input->size());

   	  spm::range_t ranges(N, parDeg);

          auto start_time = spm::timer::start();

          /*******************************************************************/
          auto block_prefix = [&](const unsigned int i)
          {
            auto [a, b] = ranges(i);
      	    output[a] = (*input)[a];
      	    for(++a; a<b; ++a) output[a] = op((*input)[a], output[a-1]);
          };

          std::vector<std::thread> threads_prefix;

          for(unsigned int i=0; i<ranges.blocks(); ++i)
            threads_prefix.emplace_back(block_prefix, i);

          for(auto &t : threads_prefix)
            t.join();

          auto step1 = spm::timer::step(start_time);
          /*******************************************************************/
          std::vector<T> block_sum(ranges.blocks());
          block_sum[0] = output[std::get<1>(ranges(0))-1];

          for(unsigned int i=1; i<ranges.blocks(); ++i)
          {
            T el = output[std::get<1>(ranges(i))-1];
            block_sum[i] = op(block_sum[i-1], el);
          }

          auto step2 = spm::timer::step(start_time);
          /*******************************************************************/
          auto block_add = [&](const unsigned int i)
          {
            auto [a, b] = ranges(i);
            for(; a<b; ++a)
              output[a] = op(output[a], block_sum[i-1]);
          };

          std::vector<std::thread> threads_sum;

          for(unsigned int i=1; i<ranges.blocks(); ++i)
            threads_sum.emplace_back(block_add, i);
          for(auto &t : threads_sum)
            t.join();

          auto step3 = spm::timer::step(start_time);
          /*******************************************************************/

          step3 = step3-step2;
          step2 = step2-step1;

          last_test = {step1, step2, step3};
        }

        std::array<spm::timer::ms_t, 3> getLastTest()
        {
          return last_test;
        }

    };
  }
}


#endif