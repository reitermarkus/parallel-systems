#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

static const unsigned long RESOLUTION = 120;

// -- vector utilities --

void print_temperature(vector<double> m, unsigned long n);

// -- simulation code ---

int main(int argc, char **argv) {
  // 'parsing' optional input parameter = problem size
  auto n = 2000;
  if (argc > 1) {
    n = strtol(argv[1], NULL, 10);
  }
  auto time_steps = n * 500;
  printf("Computing heat-distribution for room size n=%d for time_steps=%d timesteps\n", n, time_steps);

  // ---------- setup ----------

  // create a buffer for storing temperature fields
  vector<double> buffer_a(n);

  // set up initial conditions in buffer_a
  for (auto i = 0; i < n; i++) {
    buffer_a[i] = 273; // temperature is 0° C everywhere (273 K)
  }

  // and there is a heat source in one corner
  auto source_x = n / 4;
  buffer_a[source_x] = 273 + 60;

  print_temperature(buffer_a, n);
  cout << " initial" << endl;

  // ---------- compute ----------

  // create a second buffer for the computation
  vector<double> buffer_b(n);

  // for each time step ..
  for (auto t = 0; t < time_steps; t++) {
    // .. we propagate the temperature
    for (auto i = 0; i < n; i++) {
      // center stays constant (the heat is still on)
      if (i == source_x) {
        buffer_b[i] = buffer_a[i];
        continue;
      }

      // get temperature at current position
      double tc = buffer_a[i];

      // get temperatures of adjacent cells
      double tl = (i != 0) ? buffer_a[i - 1] : tc;
      double tr = (i != n - 1) ? buffer_a[i + 1] : tc;

      // compute new temperature at current position
      buffer_b[i] = tc + 0.2 * (tl + tr + (-2 * tc));
    }

    // swap matrices (just pointers, not content)
    swap(buffer_a, buffer_b);

    // show intermediate step
    if (t % 1000 == 0) {
      print_temperature(buffer_a, n);
      cout << " t=" << t << endl;
    }
  }

  // ---------- check ----------

  print_temperature(buffer_a, n);
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
