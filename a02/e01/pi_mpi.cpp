#include <cerrno>
#include <cstring>
#include <chrono>
#include <functional>
#include <memory>
#include <iostream>
#include <random>
#include <string>

#include "../parse_ull.hpp"
#include "../boost.hpp"

using namespace std;

int main(int argc, char **argv) {
  auto samples = 1000000000;

  if (argc > 1) {
    samples = parse_ull(argv[1]);
  }

  auto start_time = chrono::high_resolution_clock::now();

  mpi::environment env(argc, argv);

  mpi::communicator world;
  int size = world.size();
  int rank = world.rank();

  if (rank == 0) {
    cout << "┌──────┬──────────────────────┬──────────────────────┬──────────────────────┐" << endl;
    cout << "❘ Rank ❘                Start ❘                 Stop ❘                 Time ❘" << endl;
    cout << "├──────┼──────────────────────┼──────────────────────┼──────────────────────┤" << endl;
  }

  world.barrier();

  random_device rd;
  mt19937 gen(rd());
  uniform_real_distribution<> dis(-1, 1);

  unsigned long long inside = 0;
  auto local_inside = inside;

  auto chunk_size = (samples + (size - 1)) / size;
  auto start = chunk_size * rank;
  auto stop = start + chunk_size;
  if (stop > samples) {
    stop = samples;
  }

  for (auto i = start; i < stop; i++) {
    auto x = dis(gen);
    auto y = dis(gen);

    if (x * x + y * y <= 1.0) {
      local_inside++;
    }
  }

  mpi::reduce(world, local_inside, inside, plus<unsigned long long>(), 0);

  auto end_time = chrono::high_resolution_clock::now();
  chrono::milliseconds duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

  printf("| %4d | %20d | %20d | %17lld ms |\n", rank, start, stop, (long long int)duration.count());

  world.barrier();

  if (rank == 0) {
    cout << "└──────┴──────────────────────┴──────────────────────┴──────────────────────┘" << endl;
    cout << endl;

    double pi = (double)inside / (double)samples * 4.0;
    cout << "π ≈ " << pi << endl;
  }

  return EXIT_SUCCESS;
}

