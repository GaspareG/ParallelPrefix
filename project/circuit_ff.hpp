/*
        filename: circuit_ff.hpp
          author: Gaspare Ferraro <ferraro@gaspa.re>
  student number: 520549
          degree: MD in CS: Data and Knowledge Science and Technologies
            exam: Parallel and Distributed Systems: Paradigms and Models

     description: Implementation with FastFlow framework of the circuit parallel prefix computation

     class usage:

      // Create object
      spm::circuit::parallelPrefixFF<T, op> pp_ff(input, parDegree);
      // T type of elements in vectors
      // op binary function (T,T) -> T (commutative and associative)
      // input = shared pointer of input vector (of type T)
      // parDegree = number of worker for the computation

      // Change parallelism degree of the computation
      pp_ff.setParallelismDegree(np);
      // np = new parallism degree

      // Start computation of parallel prefix vector
      pp_ff.start(output);
      // output = reference to the output vector (we assume input.size() >= output.size())

      // Get time statistic about last computation
      auto [t1, t2, t3] = pp_ff.getLastTest();
      // t1 = msec time of first phase
      // t2 = msec time of second phase
      // t3 = msec time of third phase
*/

#ifndef CIRCUIT_FF_HPP
#define CIRCUIT_FF_HPP

#include <vector>
#include <memory>
#include <array>
#include <algorithm>
#include <utility>
#include <numeric>
#include <ff/parallel_for.hpp>
#include "utils/clock.hpp"
#include "utils/circuit.hpp"

namespace spm
{
  namespace circuit
  {
    template<class T, T op(T, T)>
    class parallelPrefixFF
    {

    private:

      std::shared_ptr <std::vector<T>> input;
      std::array<spm::timer::ms_t, 3> last_test;
      unsigned int parDeg;

    public:

      parallelPrefixFF(std::shared_ptr <std::vector<T>> const &in, unsigned int np) : input(in), parDeg(np)
      {}

      void setParallelismDegree(unsigned int np)
      {
        parDeg = np;
      }

      void start(std::vector <T> &output)
      {

        // assert(in.size() == out.size())

        int n = static_cast<int>(input->size());
        int m = 1; // m = log2(n);

        while ((1 << m) < n) m++;

        // assert((1<<m) == n);

        auto start_time = spm::timer::start();

        // FastFlow parallel for
        ff::ParallelFor pf(parDeg);

        /*******************************************************************/
        // First phase

        // In the first task we need also to copy all the value from input to output
        // Then in second and third phase we can work in place
        pf.parallel_for(1, spm::circuit::k1(1, m) + 1, [&](const long int k)
        {
          auto[l, r] = spm::circuit::g1(1, k);
          output[l] = (*input)[l];
          output[r] = op((*input)[l], (*input)[r]);
        });

        auto step1 = spm::timer::step(start_time);

        /*******************************************************************/
        // Second phase

        for (int t = 2; t <= m; t++)
        {
          pf.parallel_for(1, spm::circuit::k1(t, m) + 1, [&](const long int k)
          {
            auto[l, r] = spm::circuit::g1(t, k);
            output[r] = op(output[l], output[r]);
          });
        }

        auto step2 = spm::timer::step(start_time);

        /*******************************************************************/
        // Third phase

        for (int t = 1; t < m; t++)
        {
          pf.parallel_for(1, spm::circuit::k2(t) + 1, [&](const long int k)
          {
            auto[l, r] = spm::circuit::g2(t, k, m);
            output[r] = op(output[l], output[r]);
          });
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
