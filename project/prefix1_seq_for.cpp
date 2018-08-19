#include <iostream>
#include <vector>

int main(int argc, char **argv)
{
  if( argc < 1 )
  {
    std::cout << "Usage: " << argv[0] << " N" << std::endl;
    exit(1);
  }
  int N = atoi(argv[1]);
  std::vector<int> V(N);

  // Read input
  for(int i=0; i<N; i++)
  {
    int tmp;
    std::cin >> tmp;
    V[i] = tmp;
  }

  // Calc prefix array
  for(int i=1; i<N; i++)
    V[i] += V[i-1];

  // Write output
  for(int i : V)
    std::cout << i << " ";
  std::cout << std::endl;

  return 0;
}
