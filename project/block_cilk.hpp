/*
        filename: block_cilk.hpp
          author: Gaspare Ferraro <ferraro@gaspa.re>
  student number: 520549
          degree: MD in CS: Data and Knowledge Science and Technologies
            exam: Parallel and Distributed Systems: Paradigms and Models

     description: Implementation with cilk extension of the block parallel prefix computation

     class usage:

      // Create object
      spm::block::parallelPrefixCilk<T, op> pp_cilk(input, parDegree);
      // T type of elements in vectors
      // op binary function (T,T) -> T (commutative and associative)
      // input = shared pointer of input vector (of type T)
      // parDegree = number of worker for the computation

      // Change parallelism degree of the computation
      pp_clik.setParallelismDegree(np);
      // np = new parallism degree

      // Start computation of parallel prefix vector
      pp_cilk.start(output);
      // output = reference to the output vector (we assume input.size() >= output.size())

      // Get time statistic about last computation
      auto [t1, t2, t3] = pp_cilk.getLastTest();
      // t1 = msec time of first phase
      // t2 = msec time of second phase
      // t3 = msec time of third phase
*/

#ifndef BLOCK_CILK_HPP
#define BLOCK_CILK_HPP

#include <vector>
#include <memory>
#include <array>
#include <algorithm>
#include <utility>
#include <numeric>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "utils/clock.hpp"

namespace spm
{
  namespace block
  {
    template<class T, T op(T, T)>
    class parallelPrefixCilk
    {

    private:

      std::shared_ptr <std::vector<T>> input;
      std::array<spm::timer::ms_t, 3> last_test;
      unsigned int parDeg;

    public:

      parallelPrefixCilk(std::shared_ptr <std::vector<T>> const &in, unsigned int np) : input(in), parDeg(np)
      {}

      void setParallelismDegree(unsigned int np)
      {
        parDeg = np;
      }

      void start(std::vector <T> &output)
      {

        // assert(in.size() == out.size())

        // Create range
        unsigned int N = static_cast<unsigned int>(input->size());
        spm::range_t ranges(N, parDeg);

        auto start_time = spm::timer::start();

        __cilkrts_end_cilk();
        __cilkrts_set_param("nworkers", (std::to_string(parDeg)).c_str());

        /*******************************************************************/
        // First phase

        // Given a range i compute prefix sum over block i
        auto block_prefix = [&](const unsigned int i)
        {
          auto[a, b] = ranges(i);
          output[a] = (*input)[a];
          for (++a; a < b; ++a) output[a] = op((*input)[a], output[a - 1]);
        };

        // Cilk_for execute one call to block_prefix per thread
        cilk_for (unsigned int i = 0; i < ranges.blocks(); ++i)
        {
          block_prefix(i);
        }

        auto step1 = spm::timer::step(start_time);

        /*******************************************************************/
        // Second phase

        // Compute prefix sum over the end of each block
        std::vector <T> block_sum(parDeg);
        block_sum[0] = output[std::get<1>(ranges(0)) - 1];

        for (unsigned int i = 1; i < parDeg; ++i)
        {
          T el = output[std::get<1>(ranges(i)) - 1];
          block_sum[i] = op(block_sum[i - 1], el);
        }

        auto step2 = spm::timer::step(start_time);

        /*******************************************************************/
        // Third phase

        // Given a range i ( > 0 ) add block_sum[i-1] over all block i
        auto block_add = [&](const unsigned int i)
        {
          auto[a, b] = ranges(i);
          for (; a < b; ++a)
            output[a] = op(output[a], block_sum[i - 1]);
        };

        // Cilk_for execute one call to block_prefix per thread
        cilk_for (unsigned int i = 1; i < ranges.blocks(); ++i)
        {
          block_add(i);
        }
        auto step3 = spm::timer::step(start_time);

        /*******************************************************************/

        // Update last time statistic
        step3 = step3 - step2;
        step2 = step2 - step1;
        last_test = {step1, step2, step3};

      }

      std::array<spm::timer::ms_t, 3> getLastTest()
      {
        return last_test;
      }

      spm::timer::ms_t getLastTime()
      {
        spm::timer::ms_t out = 0;
        for (auto t : last_test) out += t;
        return out;
      }

    };
  }
}


#endif
