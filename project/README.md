To compile the project just run the script compile.sh

Requirements:
- C++17 support
- OpenMP
- FastFlow (already in folder, or $FF_ROOT defined in enviroment)
- Cilk plus extension

If the compile goes right the script print the help of the benchmark:

```
Benchmark of all parallel-prefix implementations
Usage:
  benchmark [OPTION...]

  -h, --help         Print help
  -c, --complete     Execute smart benchmark (default: false)
      --stl-seq      Test STL sequential implementation
      --stl-block    Test STL block implementation
      --stl-circ     Test STL circuit implementation
      --ff-block     Test FF block implementation
      --ff-pipe      Test FF pipeline block implementation
      --ff-circ      Test FF circuit implementation
      --omp-block    Test OMP block implementation
      --omp-circ     Test OMP circuit implementation
      --cilk-block   Test CILK block implementation
      --cilk-circ    Test CILK circuit implementation
      --all-block    Test all block implementations
      --all-circuit  Test all circuit implementations
      --all          Test all implementations
      --exp arg      N° of experiments to run (default: 1)
      --par-min arg  Minimum parallelism degree (default: 1)
      --par-max arg  Maximum parallelism degree (default: max)
      --par-lin      Linear step in par degree (default: false)
      --check        Check for errors (default: false)
      --dataset arg  Input dataset (default 0: rnd, 1: 1s, 2: incr)
      --seed arg     Seed for random number generator (default 42)
      --m-min arg    Min log2 of vector size (default: 25)
      --m-max arg    Max log2 of vector size (default: 28)
```
