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

static const unsigned long RESOLUTION = 120;

void print_temperature(vector<double> m, unsigned long n);

int main(int argc, char **argv) {
  auto room_size = 2000;

  if (argc > 1) {
    room_size = parse_ull(argv[1]);
  }

  auto time_steps = room_size * 500;
  printf("Computing heat-distribution for room size %d for %d timesteps\n", room_size, time_steps);

  mpi::environment env(argc, argv);

  mpi::communicator world;
  int size = world.size();
  int rank = world.rank();

  // Create buffers for storing temperature fields and
  // initialize temperature to 0° C (273 K) everywhere.
  vector<double> buffer_a(room_size, 273);
  vector<double> buffer_b(room_size);

  // Place a heat source in one corner.
  auto source_x = room_size / 4;
  buffer_a[source_x] = 273 + 60;

  print_temperature(buffer_a, room_size);
  cout << " initial" << endl;

  // Propagate the temperature in each time step.
  for (auto t = 0; t < time_steps; t++) {
    for (auto i = 0; i < room_size; i++) {
      // The center stays constant (the heat is still on).
      if (i == source_x) {
        buffer_b[i] = buffer_a[i];
        continue;
      }

      // Get temperature at current position.
      double temp_current = buffer_a[i];

      // Get temperatures of adjacent cells.
      double temp_left = (i != 0) ? buffer_a[i - 1] : temp_current;
      double temp_right = (i != room_size - 1) ? buffer_a[i + 1] : temp_current;

      // Compute new temperature at current position.
      buffer_b[i] = temp_current + 0.2 * (temp_left + temp_right + (-2 * temp_current));
    }

    // Swap matrices (just pointers, not content).
    swap(buffer_a, buffer_b);

    // Show intermediate step.
    if (t % 1000 == 0) {
      print_temperature(buffer_a, room_size);
      cout << " t=" << t << endl;
    }
  }

  // ---------- check ----------

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
