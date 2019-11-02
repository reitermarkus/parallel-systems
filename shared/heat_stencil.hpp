#pragma once

#include <iostream>
#include <string>

using namespace std;

static const string COLORS = " .-:=+*^X#%@";

// boundaries for temperature (for simplicity hard-coded)
static const float MAX_TEMP = 273.0 + 30.0;
static const float MIN_TEMP = 273.0 +  0.0;

void print_color(const float temp) {
  static size_t num_colors = COLORS.length();

  // pick the 'color'
  size_t c = clamp<ssize_t>(((temp - MIN_TEMP) / (MAX_TEMP - MIN_TEMP)) * static_cast<float>(num_colors), 0, num_colors - 1);

  // print the average temperature
  cout << COLORS[c];
}

void print_wall(const string begin, const string end, const size_t w) {
  cout << begin;

  for (size_t i = 0; i < w; i++) {
    cout << "─";
  }

  cout << end << endl;
}

void print_upper_wall(size_t w) {
  print_wall("┌", "┐", w);
}

void print_lower_wall(size_t w) {
  print_wall("└", "┘", w);
}

void print_right_wall() {
  cout << "│" << endl;
}

void print_left_wall() {
  cout << "│";
}
