#include <bits/stdc++.h>

int plus(int x, int y)
{
  return x^y;
}

void prefixSum(const std::vector<int>& V, std::vector<int>& V2, int NW)
{
  int N = static_cast<int>(V.size());

  // Create ranges
  int blockSize = N / NW;

  std::cerr << "NW = " << NW << std::endl;

  std::vector<std::tuple<int,int>> ranges(NW);
  for(int i=0; i<NW; i++) ranges[i] = std::make_tuple(i*blockSize, std::min(N, (i+1)*blockSize));
  std::get<1>(ranges[NW-1]) = N;

  auto start   = std::chrono::high_resolution_clock::now();

  // First phase: parallel block partial_sum
  std::vector<std::thread> threads_b;
  auto block_f = [&V, &V2](std::tuple<int,int> range){
    int a = std::get<0>(range);
    int b = std::get<1>(range);
    std::partial_sum(std::begin(V)+a, std::begin(V)+b, std::begin(V2)+a);
  };

  for(int i=0; i<NW; i++) threads_b.push_back(std::thread(block_f, ranges[i]));
  for(auto &t : threads_b) t.join();
  // end

  auto start1 = std::chrono::high_resolution_clock::now();
  auto phase1 = start1 - start;

  // Second phase: sequential partial_sum on end-ranges
  std::vector<int> p_sum(NW, 0);
  {
    for(int i=1; i<NW; i++) p_sum[i] = p_sum[i-1] + V2[std::get<1>(ranges[i-1])-1];
  }
  // end

  auto start2 = std::chrono::high_resolution_clock::now();
  auto phase2 = start2 - start1;

  // Third phase: parallel range add
  {
    std::vector<std::thread> threads_a;
    auto add_f = [&V2](std::tuple<int,int> range, int add){
      int a = std::get<0>(range);
      int b = std::get<1>(range);
      std::transform(std::begin(V2)+a, std::begin(V2)+b, std::begin(V2)+a, [add](int x){ return x+add; });
    };

    for(int i=0; i<NW; i++) threads_a.push_back(std::thread(add_f, ranges[i], p_sum[i]));
    for(auto &t : threads_a) t.join();
  }
  // end

  auto start3 = std::chrono::high_resolution_clock::now();
  auto phase3 = start3 - start2;

  auto elapsed = std::chrono::high_resolution_clock::now() - start;

  auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

  auto usec1    = std::chrono::duration_cast<std::chrono::milliseconds>(phase1).count();
  auto usec2    = std::chrono::duration_cast<std::chrono::milliseconds>(phase2).count();
  auto usec3    = std::chrono::duration_cast<std::chrono::milliseconds>(phase3).count();

  std::cout << NW << " " << N << " " << usec << std::endl;
  std::cout << "\tphase 1: " << usec1 << std::endl;
  std::cout << "\tphase 2: " << usec2 << std::endl;
  std::cout << "\tphase 3: " << usec3 << std::endl;

  return;
}

int main()
{
  std::vector<int> V(1<<30);
  std::vector<int> V2(1<<30);

  std::iota(std::begin(V), std::end(V), 0);

  unsigned int max_thread = std::thread::hardware_concurrency();

  for(int i=1; i<=max_thread; i*=2)
  {
    prefixSum(V, V2, i);
    std::cout << V2[(1<<30)-1] << std::endl;
  }

  auto start   = std::chrono::high_resolution_clock::now();
  std::partial_sum(std::begin(V), std::end(V), std::begin(V));
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

  std::cout << "Sequantial partial_sum: " << usec << std::endl;

  std::cout << "diff " << (
  return 0;
}
