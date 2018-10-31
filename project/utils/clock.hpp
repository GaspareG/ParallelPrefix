#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <chrono>

namespace spm
{
  namespace timer
  {

    auto start()
    {
      return std::chrono::high_resolution_clock::now();
    }

    auto step(auto timer_now)
    {
      return std::chrono::duration_cast<std::chrono::milliseconds>(start() - timer_now).count();
    }

    using ms_t = decltype(step(start()));

  }
}

#endif

