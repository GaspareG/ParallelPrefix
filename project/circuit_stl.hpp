/*
        filename: circuit_stl.hpp
          author: Gaspare Ferraro <ferraro@gaspa.re>
  student number: 520549
          degree: MD in CS: Data and Knowledge Science and Technologies
            exam: Parallel and Distributed Systems: Paradigms and Models

     description: Implementation with STL Threads of the circuit parallel prefix computation

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
          spm::range_t ranges_p1(spm::circuit::k1(1, m), parDeg);

          auto circuit_f1 = [&](const unsigned int i)
          {
            auto [a, b] = ranges_p1(i);
            a++;
            b++;
            for(a; a<b; a++)
            {
              auto [l, r] = spm::circuit::g1(1, a);
              output[l] = (*input)[l];
              output[r] = op((*input)[l], (*input)[r]);
            }
          };

          std::vector<std::thread> threads_p1;

          for(unsigned int i=0; i<ranges_p1.blocks(); ++i)
            threads_p1.emplace_back(circuit_f1, i);

          for(auto &t : threads_p1)
            t.join();

          auto step1 = spm::timer::step(start_time);
          /*******************************************************************/
          std::vector<std::thread> threads_p2;
          spm::range_t ranges_p2(spm::circuit::k1(2, m), parDeg);

          auto circuit_f2 = [&](const unsigned int i, const int t)
          {
            auto [a, b] = ranges_p2(i);
            a++;
            b++;
            for(int k=a; k<b; k++)
            {
              auto [l, r] = spm::circuit::g1(t, k);
              output[r] = op(output[l], output[r]);
            }
          };

          for(int t=2; t<=m; t++)
          {
            ranges_p2 = spm::range_t(spm::circuit::k1(t, m), parDeg);

            for(unsigned int i=0; i<ranges_p2.blocks(); ++i)
              threads_p2.emplace_back(circuit_f2, i, t);

            for(auto &t : threads_p2)
              t.join();

            threads_p2.clear();
          }

          auto step2 = spm::timer::step(start_time);
          /*******************************************************************/
          std::vector<std::thread> threads_p3;
          spm::range_t ranges_p3(spm::circuit::k2(1), parDeg);

          auto circuit_f3 = [&](const unsigned int i, const int t, const int m)
          {
            auto [a, b] = ranges_p3(i);
            a++;
            b++;
            for(int k=a; k<b; k++)
            {
              auto [l, r] = spm::circuit::g2(t, k, m);
              output[r] = op(output[l], output[r]);
            }
          };

          for(int t=1; t<m; t++)
          {
            ranges_p3 = spm::range_t(spm::circuit::k2(t), parDeg);

            for(unsigned int i=0; i<ranges_p3.blocks(); ++i)
              threads_p3.emplace_back(circuit_f3, i, t, m);

            for(auto &t : threads_p3)
              t.join();

            threads_p3.clear();
          }

          auto step3 = spm::timer::step(start_time);
          /*******************************************************************/

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
          for(auto t : last_test) out += t;
          return out;
        }

    };
  }
}


#endif
