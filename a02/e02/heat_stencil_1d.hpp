#pragma once

#include "../../shared/heat_stencil.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

inline void print_temperature(const vector<float>& m) {
  // set the 'render' resolution
  auto w = 120;

  // step size in each dimension
  auto step_size = m.size() / w;

  print_upper_wall(w);

  print_left_wall();

  for (size_t i = 0; i < w; i++) {
    // get max temperature in this tile
    float max_temp = 0;
    for (auto x = step_size * i; x < step_size * i + step_size; x++) {
      max_temp = max(max_temp, m[x]);
    }

    print_color(max_temp);
  }

  print_right_wall();

  print_lower_wall(w);
}
