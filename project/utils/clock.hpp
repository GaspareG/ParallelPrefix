/*
        filename: clock.hpp
          author: Gaspare Ferraro <ferraro@gaspa.re>
  student number: 520549
          degree: MD in CS: Data and Knowledge Science and Technologies
            exam: Parallel and Distributed Systems: Paradigms and Models

     description: Handler for chrono functions

*/
#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <chrono>

namespace spm
{
  namespace timer
  {

    // Return current time
    auto start()
    {
      return std::chrono::high_resolution_clock::now();
    }

    // Return msec pass from timer_now
    auto step(auto timer_now)
    {
      return std::chrono::duration_cast<std::chrono::milliseconds>(start() - timer_now).count();
    }

    // Duration type
    using ms_t = decltype(step(start()));

  }
}

#endif

