#pragma once

#include <vector>
#include <iostream>

using namespace std;

struct Queen {
  size_t row;
  size_t column;

  Queen(size_t row, size_t column) {
    this->row = row;
    this->column = column;
  }

  bool can_be_killed_by(const vector<Queen>& other_queens) {
    for (auto queen: other_queens) {
      ssize_t row_diff = abs(ssize_t(queen.row) - ssize_t(this->row));
      ssize_t col_diff = abs(ssize_t(queen.column) - ssize_t(this->column));

      // horizontal || diagonal
      if (row_diff == 0 || row_diff == col_diff) {
        return true;
      }
    }

    return false;
  }
};

void print(const vector<size_t>& board, int queens) {
  for (int i = 0; i < queens; i++) {
    for (int j = 0; j < queens; j++)
      cout << (board[i * queens + j] ? "⬛︎" : "⬜︎");

    cout << endl;
  }
}
