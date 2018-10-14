#include <bits/stdc++.h>

int plus(int x, int y)
{
  return x^y;
}

void prefixSum(const std::vector<int>& V, int NW)
{
  int N = static_cast<int>(V.size());

  std::vector<int> V2(N);

  auto start   = std::chrono::high_resolution_clock::now();

  std::vector<std::tuple<int,int>> ranges(NW);
  int blockSize = N / NW;

  for(int i=0; i<NW; i++)
    ranges[i] = std::make_tuple(i*NW, std::min(N, (i+1)*NW));

  std::vector<std::thread> threads;

  auto block_f = [&V, &V2](std::tuple<int,int> range){
    int a = std::get<0>(range);
    int b = std::get<1>(range);
    V2[a] = V[a];
    for(int i=a+1; i<b; i++)
      V2[i] = plus(V2[i-1], V[i]);
  };

  for(int i=0; i<NW; i++)
    threads.push_back(std::thread(block_f, ranges[i]));

  for(auto &t : threads)
    t.join();

  std::vector<int> p_sum(NW, 0);
  for(int i=1; i<NW; i++)
    p_sum[i] = p_sum[i-1] + V2[std::get<1>(ranges[i-1])];

  auto add_f = [&V2](std::tuple<int,int> range, int add){
    int a = std::get<0>(range);
    int b = std::get<1>(range);
    for(int i=a; i<b; i++)
      V2[i] += add;
  };

  threads.clear();

  for(int i=0; i<NW; i++)
    threads.push_back(std::thread(add_f, ranges[i], p_sum[i]));

  for(auto &t : threads)
    t.join();

  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

  std::cout << NW << " " << N << " " << usec<< std::endl;

  return;
}

int main()
{
  std::vector<int> V(1<<30);
  std::iota(std::begin(V), std::end(V), 0);

  for(int i=1; i<8; i++)
    prefixSum(V, i);

  return 0;
}
