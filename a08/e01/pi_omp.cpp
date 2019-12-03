#include <cerrno>
#include <cstring>
#include <iostream>
#include <random>

#include <omp.h>

#include "../../shared/parse_ull.hpp"

using namespace std;

int main(int argc, char **argv) {
  size_t samples = 100000000;

  if (argc > 1) {
    samples = parse_ull(argv[1]);
  }

  random_device rd;
  uniform_real_distribution<> dis(-1, 1);

  size_t inside = 0;

  #pragma omp parallel reduction(+:inside)
  {
    unsigned int seed = 0;
    #ifdef DEBUG
      seed = omp_get_thread_num();
    #else
      #pragma omp critical
      seed = rd();
    #endif
    mt19937 gen(seed);

    #pragma omp for
    for (size_t i = 1; i <= samples; i++) {
      auto x = dis(gen);
      auto y = dis(gen);

      if (x * x + y * y <= 1.0) {
        inside++;
      }
    }
  }

  cout << "\rπ = " << (double)inside / (double)samples * 4.0 << endl;

  return EXIT_SUCCESS;
}

