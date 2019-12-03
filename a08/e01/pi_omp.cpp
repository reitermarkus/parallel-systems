#include <cerrno>
#include <cstring>
#include <iostream>
#include <random>

#include "../../shared/parse_ull.hpp"

using namespace std;

int main(int argc, char **argv) {
  size_t samples = 100000000;

  if (argc > 1) {
    samples = parse_ull(argv[1]);
  }

  mt19937 gen(1);
  uniform_real_distribution<> dis(-1, 1);

  size_t inside = 0;

  #pragma omp parallel private(gen, dis) reduction(+:inside)
  {
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

