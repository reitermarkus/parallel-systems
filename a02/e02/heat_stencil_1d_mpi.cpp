#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#pragma GCC diagnostic push
#if !__clang__
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <boost/mpi.hpp>
#pragma GCC diagnostic pop

#include "../parse_ull.hpp"

using namespace std;

namespace mpi = boost::mpi;

static const unsigned long RESOLUTION = 120;

void print_temperature(vector<double> m, unsigned long n);

template<typename T>
std::ostream & operator<<(std::ostream & os, std::vector<T> vec)
{
    os<<"{ ";
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(os, " "));
    os<<"}";
    return os;
}

int main(int argc, char **argv) {
  auto room_size = 2000;

  if (argc > 1) {
    room_size = parse_ull(argv[1]);
  }

  mpi::environment env(argc, argv);

  mpi::communicator world;
  int size = world.size();
  int rank = world.rank();

  auto time_steps = room_size * 500;
  if (rank == 0) {
    printf("Computing heat-distribution for room size %d for %d timesteps\n", room_size, time_steps);
  }

  auto chunk_size = (room_size + (size - 1)) / size;
  auto last_chunk_size = room_size - ((room_size - chunk_size) / chunk_size) * chunk_size;
  auto buffer_size = rank == size - 1 ? last_chunk_size : chunk_size;

  // Create buffers for storing temperature fields and
  // initialize temperature to 0° C (273 K) everywhere.
  vector<double> buffer_a(buffer_size, 273);
  vector<double> buffer_b(buffer_size);

  // Place a heat source in one corner.
  auto source_x = room_size / 4;
  auto source_rank = source_x / chunk_size;
  auto source_index = source_x % chunk_size;

  if (rank == source_rank) {
    cout << "Heat source is at rank " << source_rank << ", index " << source_index << "." << endl;
    buffer_a[source_index] += 60;
  }

  // Propagate the temperature in each time step.
  for (auto t = 0; t < time_steps; t++) {
    if (rank > 0) {
      // Send leftmost temperature to left neighbor.
      world.isend(rank - 1, 1, &buffer_a[0], 1);
    }

    if (rank < size - 1) {
      // Send rightmost temperature to right neighbor.
      world.isend(rank + 1, 2, &buffer_a[chunk_size - 1], 1);
    }

    for (auto i = 0; i < buffer_size; i++) {
      // The center stays constant (the heat is still on).
      if (rank == source_rank && i == source_index) {
        buffer_b[i] = buffer_a[i];
        continue;
      }

      // Get temperature at current position.
      double temp_current = buffer_a[i];

      // Get temperatures from left neighbor cell.
      double temp_left;

      if (i == 0) {
        if (rank == 0) {
          temp_left = temp_current;
        } else {
          world.recv(rank - 1, 2, &temp_left, 1);
        }
      } else {
        temp_left = buffer_a[i - 1];
      }

      // Get temperatures from right neighbor cell.
      double temp_right;

      if (i == buffer_size - 1) {
        if (rank == size - 1) {
          temp_right = temp_current;
        } else {
          world.recv(rank + 1, 1, &temp_right, 1);
        }
      } else {
        temp_right = buffer_a[i + 1];
      }

      // Compute new temperature at current position.
      buffer_b[i] = temp_current + 0.2 * (temp_left + temp_right + (-2.0 * temp_current));
    }

    // Swap matrices (just pointers, not content).
    swap(buffer_a, buffer_b);
  }

  if (rank == 0) {
    buffer_a.resize(room_size);

    for (auto i = 1; i < size; i++) {
      world.recv(i, 3, &buffer_a[0] + chunk_size * i, i == size - 1 ? last_chunk_size : chunk_size);
    }
  } else {
    world.send(0, 3, &buffer_a[0], buffer_size);
  }

  // ---------- check ----------

  if (rank > 0) {
    return EXIT_SUCCESS;
  }

  print_temperature(buffer_a, room_size);
  cout << " final" << endl;

  cout << "Verification: ";

  for (auto temp : buffer_a) {
    if (temp < 273 || temp > 273 + 60) {
      cout << "FAILED" << endl;
      return EXIT_FAILURE;
    }
  }

  cout << "OK" << endl;
  return EXIT_SUCCESS;
}

void print_temperature(vector<double> m, unsigned long n) {
  const string colors(" .-:=+*^X#%@");

  // boundaries for temperature (for simplicity hard-coded)
  const auto max = 273 + 30;
  const auto min = 273 + 0;

  // set the 'render' resolution
  auto resolution = RESOLUTION;

  // step size in each dimension
  auto step_size = n / resolution;

  // room
  // left wall
  cout << "|";
  // actual room
  for (unsigned long i = 0; i < resolution; i++) {
    // get max temperature in this tile
    double max_temp = 0;
    for (auto x = step_size * i; x < step_size * i + step_size; x++) {
      max_temp = max_temp < m[x] ? m[x] : max_temp;
    }

    // pick the 'color'
    auto c = ((max_temp - min) / (max - min)) * colors.length();
    c = (c >= colors.length()) ? colors.length() - 1 : ((c < 0) ? 0 : c);

    // print the average temperature
    cout << colors[c];
  }
  // right wall
  cout << "|";
}
