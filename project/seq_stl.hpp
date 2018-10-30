/*
        filename: seq_stl.hpp
          author: Gaspare Ferraro <ferraro@gaspa.re>
  student number: 520549
          degree: MD in CS: Data and Knowledge Science and Technologies
            exam: Parallel and Distributed Systems: Paradigms and Models

     description: Implementation of serial parallel prefix computation

     class usage:

      // Create object
      spm::sequential::sequentialPrefixSTL<T, op> seq_stl(input);
      // T type of elements in vectors
      // op binary function (T,T) -> T (commutative and associative)
      // input = shared pointer of input vector (of type T)

      // Start computation of parallel prefix vector
      seq_stl.start(output);
      // output = reference to the output vector (we assume input.size() >= output.size())

      // Get time statistic about last computation
      auto [t1] = seq_stl.getLastTest();
      // t1 = msec time of computation
*/

#ifndef SEQ_STL_HPP
#define SEQ_STL_HPP

#include <vector>
#include <memory>
#include <array>
#include <algorithm>
#include <utility>
#include <numeric>
#include <thread>
#include "utils/clock.hpp"

namespace spm
{
  namespace sequential
  {
    template<class T, T op(T,T)> class sequentialPrefixSTL
    {

      private:

        std::shared_ptr<std::vector<T>> input;
        std::array<spm::timer::ms_t, 1> last_test;

      public:

        sequentialPrefixSTL(std::shared_ptr<std::vector<T>> const& in) : input(in) {}

        void start(std::vector<T>& output)
        {

          // assert(in.size() == out.size())

          int N = static_cast<int>(input->size());
          auto start_time = spm::timer::start();
          /*******************************************************************/
          output[0] = (*input)[0];
          for(int i=1; i<N; i++)
            output[i] = op(output[i-1], (*input)[i]);

          auto step1 = spm::timer::step(start_time);
          /*******************************************************************/

          last_test = std::array<spm::timer::ms_t, 1>{step1};
        }

        std::array<spm::timer::ms_t, 1> getLastTest()
        {
          return last_test;
        }

    };
  }
}


#endif
