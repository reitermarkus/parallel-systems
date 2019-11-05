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
    for (size_t y = 0; y < room_size; y++) {
      for (size_t x = 0; x < room_size; x++) {
        for (size_t z = 0; z < room_size; z++) {
          // The center stays constant (the heat is still on).
          if (y == source_x && x == source_y && z == source_z) {
            buffer_b[y][x][z] = buffer_a[y][x][z];
            continue;
          }

          calc_temperature(x, y, z, x, y, z, room_size, buffer_a, buffer_b);
        }
      }
    }

    // Swap matrices (just pointers, not content).
    swap(buffer_a, buffer_b);
  }

  print_temperature(buffer_a);

  return verify(buffer_a);
}
