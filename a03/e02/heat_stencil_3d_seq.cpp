#include "heat_stencil.hpp"

int main(int argc, char **argv) {
  auto room_size = 20;

  if (argc > 1) {
    room_size = parse_ull(argv[1]);
  }

  auto time_steps = room_size * 10;

  cout << "Computing heat-distribution for room size " << room_size << " for " << time_steps << " timestaps\n";

  // Create buffers for storing temperature fields and
  // initialize temperature to 0° C (273 K) everywhere.
  vector<double> buffer_a(room_size * room_size * room_size, 273);
  vector<double> buffer_b(room_size * room_size * room_size);

  // Place a heat source in one corner.
  auto source_x = room_size / 4;
  auto source_y = room_size / 4;
  auto source_z = room_size / 4;

  buffer_a[source_x * source_y * room_size + source_z] = 273 + 60;

  // Propagate the temperature in each time step.
  for (int t = 0; t < time_steps; t++) {
    for (long long i = 0; i < room_size; i++) {
      for (long long j = 0; j < room_size; j++) {
        for (long long k = 0; k < room_size; k++) {

          // The center stays constant (the heat is still on).
          if (i == source_x && k == source_z && j == source_y) {
            buffer_b[i * j * room_size + k] = buffer_a[i * j * room_size + k];
            continue;
          }

          // Get temperature at current position.
          double temp_current = buffer_a[i * j * room_size + k];

          // Get temperatures of adjacent cells.
          double temp_up = (i != 0) ? buffer_a[(i - 1) * j * room_size + k] : temp_current;
          double temp_down = (i != room_size - 1) ? buffer_a[(i + 1) * j * room_size + k] : temp_current;

          double temp_left = (j != 0) ? buffer_a[i * (j - 1) * room_size + k] : temp_current;
          double temp_right = (j != room_size - 1) ? buffer_a[i * (j + 1) * room_size + k] : temp_current;

          double temp_front = (k != 0) ? buffer_a[i * j * room_size + (k - 1)] : temp_current;
          double temp_back = (k != room_size - 1) ? buffer_a[i * j * room_size + (k + 1)] : temp_current;

          // Compute new temperature at current position.
          buffer_b[i * j * room_size + k] = temp_current + 0.2 * (temp_left + temp_right + temp_up + temp_down + temp_front + temp_back + (-6 * temp_current));
        }
      }
    }

    // Swap matrices (just pointers, not content).
    swap(buffer_a, buffer_b);
  }

  for (auto i = 0; i < room_size; i++) {
    for (auto k = 0; k < room_size; k++) {
      for (auto j = 0; j < room_size; j++) {
        auto temp = buffer_a[i * k * room_size + j];
        if (temp < 273 || temp > 273 + 60) {
          cout << "failed at i: " << i << " " << temp << endl;
          cout << "failed at j: " << j << " " << temp << endl;
          cout << "Verification: FAILED" << endl;
          return EXIT_FAILURE;
        }
      }
    }
  }

  cout << "Verification: OK" << endl;

  return EXIT_SUCCESS;
}
