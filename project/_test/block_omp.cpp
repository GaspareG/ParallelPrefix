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

  auto start   = std::chrono::high_resolution_clock::now();

  // First phase: parallel block partial_sum
  #pragma omp parallel for schedule(static)
  for(int i=0; i<NW; i++)
  {
    int a = std::get<0>(ranges[i]);
    int b = std::get<1>(ranges[i]);
    std::partial_sum(std::begin(V)+a, std::begin(V)+b, std::begin(V2)+a);
  }
  // end

  auto start1 = std::chrono::high_resolution_clock::now();
  auto phase1 = start1 - start;

  // Second phase: sequential partial_sum on end-ranges
  std::vector<int> p_sum(NW, 0);
  {
    for(int i=1; i<NW; i++) p_sum[i] = p_sum[i-1] + V2[std::get<1>(ranges[i-1])];
  }
  // end

  auto start2 = std::chrono::high_resolution_clock::now();
  auto phase2 = start2 - start1;

  // Third phase: parallel range add
  for(int i=0; i<NW; i++)
  {
    int a = std::get<0>(ranges[i]);
    int b = std::get<1>(ranges[i]);
    std::transform(std::begin(V2)+a, std::begin(V2)+b, std::begin(V2)+a, [p_sum[i]](int x){ return x+add; });
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

  std::iota(std::begin(V), std::end(V), 0);

  for(int i=1; i<=NTHREADS; i*=2) prefixSum(V, i);

  auto start   = std::chrono::high_resolution_clock::now();
  std::partial_sum(std::begin(V), std::end(V), std::begin(V));
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

  std::cout << "Sequantial partial_sum: " << usec << std::endl;

  return 0;
}
