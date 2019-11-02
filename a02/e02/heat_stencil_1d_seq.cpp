#include "heat_stencil_1d.hpp"
#include "../../shared/parse_ull.hpp"

int main(int argc, char **argv) {
  auto room_size = 2000;

  if (argc > 1) {
    room_size = parse_ull(argv[1]);
  }

  auto time_steps = room_size * 500;

  cout << "Computing heat-distribution for room size " << room_size << " for " << time_steps << " timestaps\n";

  // Create buffers for storing temperature fields and
  // initialize temperature to 0° C (273 K) everywhere.
  vector<float> buffer_a(room_size, 273);
  vector<float> buffer_b(room_size);

  // Place a heat source in one corner.
  auto source_x = room_size / 4;
  buffer_a[source_x] = 273 + 60;

  cout << "initial" << endl;
  print_temperature(buffer_a);

  // Propagate the temperature in each time step.
  for (auto t = 0; t < time_steps; t++) {
    for (auto i = 0; i < room_size; i++) {
      // The center stays constant (the heat is still on).
      if (i == source_x) {
        buffer_b[i] = buffer_a[i];
        continue;
      }

      // Get temperature at current position.
      float temp_current = buffer_a[i];

      // Get temperatures of adjacent cells.
      float temp_left = (i != 0) ? buffer_a[i - 1] : temp_current;
      float temp_right = (i != room_size - 1) ? buffer_a[i + 1] : temp_current;

      // Compute new temperature at current position.
      buffer_b[i] = temp_current + (1.0 / 3.0) * (temp_left + temp_right + (-2 * temp_current));
    }

    // Swap matrices (just pointers, not content).
    swap(buffer_a, buffer_b);

    // Show intermediate step.
    if (t % 1000 == 0) {
      cout << "t = " << t << endl;
      print_temperature(buffer_a);
    }
  }

  // ---------- check ----------

  cout << "final" << endl;
  print_temperature(buffer_a);

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
