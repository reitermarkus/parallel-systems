#include "../../shared/heat_stencil.hpp"

int main(int argc, char **argv) {
  auto room_size = 500;

  if (argc > 1) {
    room_size = parse_ull(argv[1]);
  }

  auto time_steps = room_size * 100;

  cout << "Computing heat-distribution for room size " << room_size << " for " << time_steps << " timestaps\n";

  // Create buffers for storing temperature fields and
  // initialize temperature to 0° C (273 K) everywhere.
  vector<float> buffer_a(room_size * room_size, 273);
  vector<float> buffer_b(room_size * room_size);

  // Place a heat source in one corner.
  auto source_x = room_size / 4;
  auto source_y = room_size / 4;

  buffer_a[source_x * room_size + source_y] = 273 + 60;

  cout << " initial" << endl;
  print_temperature(buffer_a, room_size, room_size);

  // Propagate the temperature in each time step.
  for (int t = 0; t < time_steps; t++) {
    for (long long i = 0; i < room_size; i++) {
      for (long long j = 0; j < room_size; j++) {
        // The center stays constant (the heat is still on).
        if (i == source_x && j == source_y) {
          buffer_b[i * room_size + j] = buffer_a[i * room_size + j];
          continue;
        }

        // Get temperature at current position.
        float temp_current = buffer_a[i * room_size + j];

        // Get temperatures of adjacent cells.
        float temp_left = (j != 0) ? buffer_a[i * room_size + (j - 1)] : temp_current;
        float temp_right = (j != room_size - 1) ? buffer_a[i * room_size + (j + 1)] : temp_current;
        float temp_up = (i != 0) ? buffer_a[(i - 1) * room_size + j] : temp_current;
        float temp_down = (i != room_size - 1) ? buffer_a[(i + 1) * room_size + j] : temp_current;

        // Compute new temperature at current position.
        buffer_b[i * room_size + j] = temp_current + 0.2 * (temp_left + temp_right + temp_up + temp_down + (-4 * temp_current));
      }
    }

    // Swap matrices (just pointers, not content).
    swap(buffer_a, buffer_b);

    // Show intermediate step.
    if (!(t % 1000) && !getenv("CI"))  {
      cout << " t=" << t << endl;
      print_temperature(buffer_a, room_size, room_size);
    }
  }

  cout << " final" << endl;
  print_temperature(buffer_a, room_size, room_size);

  for (auto i = 0; i < room_size; i++) {
    for (auto j = 0; j < room_size; j++) {
      auto temp = buffer_a[i * room_size + j];
      if (temp < 273 || temp > 273 + 60) {
        cout << "failed at i: " << i << " " << temp << endl;
        cout << "failed at j: " << j << " " << temp << endl;
        cout << "Verification: FAILED" << endl;
        return EXIT_FAILURE;
      }
    }
  }

  cout << "Verification: OK" << endl;

  return EXIT_SUCCESS;
}
