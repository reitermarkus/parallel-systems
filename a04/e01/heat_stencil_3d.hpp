#pragma once

#include "../../shared/heat_stencil.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

inline void print_temperature(const vector<vector<vector<float>>>& mat) {
  const auto width = mat[0].size();
  const auto height = mat.size();
  const auto depth = mat[0][0].size();

  // set the 'render' resolution
  const int h = 30;
  const int w = 50;
  const int d = 3;

  // step size in each dimension
  const float sh = height / static_cast<float>(h);
  const float sw = width / static_cast<float>(w);
  const float sd = depth / static_cast<float>(d);

  for (size_t k = 0; k < d; k++) {
    print_upper_wall(w);

    for (size_t i = 0; i < h; i++) {
      print_left_wall();

      for (size_t j = 0; j < w; j++) {
        // get max temperature in this tile
        float max_temp = 0;

        for (size_t y = sh * i; y < min<size_t>(sh * i + sh, height); y++) {
          for (size_t x = sw * j; x < min<size_t>(sw * j + sw, width); x++) {
            for (size_t z = sd * k; z < min<size_t>(sd * k + sd, depth); z++) {
              max_temp =  max(max_temp, mat[y][x][z]);
            }
          }
        }

        print_color(max_temp);
      }

      print_right_wall();
    }

    print_lower_wall(w);
  }
}

inline void calc_temperature(
  const size_t& x, const size_t& y, const size_t& z,
  const size_t& global_x, const size_t& global_y, const size_t& global_z,
  const size_t& room_size,
  vector<vector<vector<float>>>& buffer_a, vector<vector<vector<float>>>& buffer_b
) {
  // Get temperature at current position.
  float temp_current = buffer_a[y][x][z];

  bool first_x = global_x == 0;
  bool last_x = global_x >= room_size - 1;
  bool first_y = global_y == 0;
  bool last_y = global_y >= room_size - 1;
  bool first_z = global_z == 0;
  bool last_z = global_z >= room_size - 1;

  // Get temperatures of adjacent cells.
  float temp_left = first_x ? temp_current : buffer_a[y][x - 1][z];
  float temp_right = last_x ? temp_current : buffer_a[y][x + 1][z];
  float temp_up = first_y ? temp_current : buffer_a[y - 1][x][z];
  float temp_down = last_y ? temp_current : buffer_a[y + 1][x][z];
  float temp_front = first_z ? temp_current : buffer_a[y][x][z - 1];
  float temp_back = last_z ? temp_current : buffer_a[y][x][z + 1];

  // Compute new temperature at current position.
  buffer_b[y][x][z] = temp_current + (temp_left + temp_right + temp_up + temp_down + temp_front + temp_back - 6.0 * temp_current) / 7.0;
}

int verify(const vector<vector<vector<float>>>& buffer) {
  for (auto y: buffer) {
    for (auto x: y) {
      for (auto z: x) {
        auto temp = z;

        if (temp < 273.0 || temp > 273.0 + 60.0) {
          cout << "wrong temperature: " << temp << endl;
          cout << "Verification: FAILED" << endl;
          return EXIT_FAILURE;
        }
      }
    }
  }

  cout << "Verification: OK" << endl;

  return EXIT_SUCCESS;
}
