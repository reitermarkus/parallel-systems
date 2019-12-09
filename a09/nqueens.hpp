#pragma once

#include <vector>
#include <iostream>

using namespace std;

void print(const vector<size_t>& board, int queens) {
  for (int i = 0; i < queens; i++) {
    for (int j = 0; j < queens; j++)
      cout << (board[i * queens + j] ? "⬛︎" : "⬜︎");

    cout << endl;
  }
}
