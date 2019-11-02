#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

static const unsigned long RESOLUTION = 120;

inline void print_temperature(const vector<vector<float>>& mat, unsigned long m, unsigned long n) {
  const string colors(" .-:=+*^X#%@");

  const int num_colors = 10;

  // boundaries for temperature (for simplicity hard-coded)
  const float max = 273 + 30;
  const float min = 273 + 0;

  // set the 'render' resolution
  const int h = 30;
  const int w = 50;

  // step size in each dimension
  const int sh = m / h;
  const int sw = n / w;

  // upper wall
  for (size_t i = 0; i < w + 2; i++) {
    printf("X");
  }

  printf("\n");

  // room
  for (size_t i = 0; i < h; i++) {
    // left wall
    printf("X");
    // actual room
    for (size_t j = 0; j < w; j++) {
      // get max temperature in this tile
      float max_t = 0;
      for (size_t y = sh * i; y < sh * i + sh; y++) {
        for (size_t x = sw * j; x < sw * j + sw; x++) {
          max_t = (max_t < mat[y][x]) ? mat[y][x] : max_t;
        }
      }

      float temp = max_t;

      // pick the 'color'
      int c = ((temp - min) / (max - min)) * num_colors;
      c = (c >= num_colors) ? num_colors - 1 : ((c < 0) ? 0 : c);

      // print the average temperature
      printf("%c", colors[c]);
    }
    // right wall
    printf("X\n");
  }

  // lower wall
  for (size_t i = 0; i < w + 2; i++) {
    printf("X");
  }

  printf("\n");
}
