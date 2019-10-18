#include <cerrno>
#include <cstring>
#include <chrono>
#include <functional>
#include <memory>
#include <iostream>
#include <random>
#include <string>
#include <mpi.h>

using namespace std;
using defer = shared_ptr<void>;

void assert_mpi(string function, int error) {
  if (error == 0) {
    return;
  }

  cerr << function << " failed." << endl;
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
  auto samples = 1000000000;

  if (argc > 1) {
    errno = 0;

    samples = strtol(argv[1], nullptr, 10);

    if (errno != 0) {
      cerr << "Failed parsing '" << argv[1] << "' to number: " << strerror(errno) << endl;
      exit(EXIT_FAILURE);
    }
  }

  auto start_time = chrono::high_resolution_clock::now();

  assert_mpi("MPI_Init", MPI_Init(&argc, &argv));

  defer _(nullptr, bind([]{
    assert_mpi("MPI_Finalize", MPI_Finalize());
  }));

  int size;
  assert_mpi("MPI_Comm_size", MPI_Comm_size(MPI_COMM_WORLD, &size));

  int rank;
  assert_mpi("MPI_Comm_rank", MPI_Comm_rank(MPI_COMM_WORLD, &rank));

  if (rank == 0) {
    cout << "┌──────┬──────────────────────┬──────────────────────┬──────────────────────┐" << endl;
    cout << "❘ Rank ❘                Start ❘                 Stop ❘                 Time ❘" << endl;
    cout << "├──────┼──────────────────────┼──────────────────────┼──────────────────────┤" << endl;
  }

  assert_mpi("MPI_Barrier", MPI_Barrier(MPI_COMM_WORLD));

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

  assert_mpi("MPI_Reduce", MPI_Reduce(&local_inside, &inside, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD));

  auto end_time = chrono::high_resolution_clock::now();
  chrono::milliseconds duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

  printf("| %4d | %20d | %20d | %17lld ms |\n", rank, start, stop, (long long int)duration.count());

  assert_mpi("MPI_Barrier", MPI_Barrier(MPI_COMM_WORLD));

  if (rank == 0) {
    cout << "└──────┴──────────────────────┴──────────────────────┴──────────────────────┘" << endl;
    cout << endl;

    double pi = (double)inside / (double)samples * 4.0;
    cout << "π ≈ " << pi << endl;
  }

  return EXIT_SUCCESS;
}

