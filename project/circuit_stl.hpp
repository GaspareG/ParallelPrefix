/*
        filename: circuit_stl.hpp
          author: Gaspare Ferraro <ferraro@gaspa.re>
  student number: 520549
          degree: MD in CS: Data and Knowledge Science and Technologies
            exam: Parallel and Distributed Systems: Paradigms and Models

     description: Implementation with STL Thread of the circuit parallel prefix computation

     class usage:

      // Create object
      spm::circuit::parallelPrefixSTL<T, op> pp_stl(input, parDegree);
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

#ifndef CIRCUIT_STL_HPP
#define CIRCUIT_STL_HPP

#include <vector>
#include <memory>
#include <array>
#include <algorithm>
#include <utility>
#include <numeric>
#include <thread>
#include "utils/clock.hpp"
#include "utils/circuit.hpp"

namespace spm
{
  namespace circuit
  {
    template<class T, T op(T,T)> class parallelPrefixSTL
    {

      private:

        std::shared_ptr<std::vector<T>> input;
        std::array<spm::timer::ms_t, 3> last_test;
        unsigned int parDeg;

      public:

        parallelPrefixSTL(std::shared_ptr<std::vector<T>> const& in, unsigned int np) : input(in), parDeg(np) {}

        void setParallelismDegree(unsigned int np)
        {
          parDeg = np;
        }

        void start(std::vector<T>& output)
        {

          // assert(in.size() == out.size())

          int n = static_cast<int>(input->size());
          int m = 1; // m = log2(n);

          while((1<<m) < n) m++;

          // assert((1<<m) == n);

          auto start_time = spm::timer::start();

          /*******************************************************************/
          // #pragma omp parallel for schedule(static) num_threads(parDeg)
          for(int k=1; k<=spm::circuit::k1(1, m); k++)
          {
            auto [l, r] = spm::circuit::g1(1, k);
            output[l] = (*input[l]);
            output[r] = op((*input)[l], (*input)[r]);
          }

          auto step1 = spm::timer::step(start_time);
          /*******************************************************************/
          for(int t=2; t<=m; t++)
          {
            // #pragma omp parallel for schedule(static) num_threads(parDeg)
            for(int k=1; k<=spm::circuit::k1(t, m); k++)
            {
              auto [l, r] = spm::circuit::g1(t, k);
              output[r] = op(output[l], output[r]);
            }
          }

          auto step2 = spm::timer::step(start_time);
          /*******************************************************************/
          for(int t=1; t<m; t++)
          {
            // #pragma omp parallel for schedule(static) num_threads(parDeg)
            for(int k=1; k<=spm::circuit::k2(t); k++)
            {
              auto [l, r] = spm::circuit::g2(t, k, m);
              output[r] = op(output[l], output[r]);
            }
          }

          auto step3 = spm::timer::step(start_time);
          /*******************************************************************/

          step3 = step2 - step2;
          step2 = step2 - step1;

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
