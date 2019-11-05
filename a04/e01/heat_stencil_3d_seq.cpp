#include "heat_stencil_3d.hpp"
#include "../../shared/parse_ull.hpp"

int main(int argc, char **argv) {
  size_t room_size = 50;

  if (argc > 1) {
    room_size = parse_ull(argv[1]);
  }

  size_t time_steps = room_size * 500;

  cout << "Computing heat-distribution for room size " << room_size << " for " << time_steps << " timesteps\n";

  // Create buffers for storing temperature fields and
  // initialize temperature to 0° C (273 K) everywhere.
  vector<vector<vector<float>>> buffer_a(room_size, vector<vector<float>>(room_size, vector<float>(room_size, 273.0)));
  vector<vector<vector<float>>> buffer_b(room_size, vector<vector<float>>(room_size, vector<float>(room_size)));

  // Place a heat source in one corner.
  auto source_x = room_size / 4;
  auto source_y = room_size / 4;
  auto source_z = room_size / 4;

  buffer_a[source_y][source_x][source_z] += 60;

  // Propagate the temperature in each time step.
  for (size_t t = 0; t < time_steps; t++) {
    for (size_t i = 0; i < room_size; i++) {
      for (size_t j = 0; j < room_size; j++) {
        for (size_t k = 0; k < room_size; k++) {
          // The center stays constant (the heat is still on).
          if (i == source_x && j == source_y && k == source_z) {
            buffer_b[i][j][k] = buffer_a[i][j][k];
            continue;
          }

          // Get temperature at current position.
          float temp_current = buffer_a[i][j][k];

          // Get temperatures of adjacent cells.
          float temp_up = i == 0 ? temp_current : buffer_a[i - 1][j][k];
          float temp_down = i == room_size - 1 ? temp_current: buffer_a[i + 1][j][k];

          float temp_left = j == 0 ? temp_current : buffer_a[i][j - 1][k];
          float temp_right = j == room_size - 1 ? temp_current : buffer_a[i][j + 1][k];

          float temp_front = k == 0 ? temp_current : buffer_a[i][j][k - 1];
          float temp_back = k == room_size - 1 ? temp_current : buffer_a[i][j][k + 1];

          // Compute new temperature at current position.
          buffer_b[i][j][k] = temp_current + (temp_left + temp_right + temp_up + temp_down + temp_front + temp_back - 6.0 * temp_current) / 7.0;
        }
      }
    }

    // Swap matrices (just pointers, not content).
    swap(buffer_a, buffer_b);
  }

  print_temperature(buffer_a);

  return verify(buffer_a);
}
