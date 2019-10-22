#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "../parse_ull.hpp"

using namespace std;

static const unsigned long RESOLUTION = 120;

inline void print_temperature(const vector<double>& m, unsigned long n) {
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
  for (unsigned long i = 0; i < resolution; i++)
  {
    // get max temperature in this tile
    double max_temp = 0;
    for (auto x = step_size * i; x < step_size * i + step_size; x++)
    {
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
