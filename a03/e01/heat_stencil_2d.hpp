#pragma once

#include "../../shared/heat_stencil.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

static const unsigned long RESOLUTION = 120;

inline void print_temperature(const vector<vector<float>>& mat) {
  const auto height = mat.size();
  const auto width = mat[0].size();

  // set the 'render' resolution
  const int h = 30;
  const int w = 50;

  // step size in each dimension
  const int sh = height / h;
  const int sw = width / w;

  print_upper_wall(w);

  // room
  for (size_t i = 0; i < h; i++) {
    print_left_wall();

    for (size_t j = 0; j < w; j++) {
      // get max temperature in this tile
      float max_temp = 0;
      for (size_t y = sh * i; y < sh * i + sh; y++) {
        for (size_t x = sw * j; x < sw * j + sw; x++) {
          max_temp = max(max_temp, mat[y][x]);
        }
      }

      print_color(max_temp);
    }

    print_right_wall();
  }

  print_lower_wall(w);
}
