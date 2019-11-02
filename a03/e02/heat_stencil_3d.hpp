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
  const int sh = height / h;
  const int sw = width / w;
  const int sd = depth / d;

  for (size_t k = 0; k < d; k++) {
    print_upper_wall(w);

    for (size_t i = 0; i < h; i++) {
      print_left_wall();

      for (size_t j = 0; j < w; j++) {
        // get max temperature in this tile
        float max_temp = 0;

        for (size_t y = sh * i; y < sh * i + sh; y++) {
          for (size_t x = sw * j; x < sw * j + sw; x++) {
            for (size_t z = sd * k; z < sd * k + sd; z++) {
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
