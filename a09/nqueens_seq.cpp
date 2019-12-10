#include <vector>

#include "../shared/parse_ull.hpp"

#include "nqueens.hpp"

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

size_t solve(
  #if DEBUG
    vector<size_t>& board,
  #endif
  size_t queens
) {
  vector<Queen> placed_queens;
  placed_queens.reserve(queens);

  size_t solutions = 0;

  auto current_queen = Queen(0, 0);

  while (current_queen.row < queens && current_queen.column < queens) {
    if (current_queen.can_be_killed_by(placed_queens)) {
      current_queen.row++;

      while (current_queen.row == queens && current_queen.column > 0) {
        Queen previous_queen = placed_queens.back();
        placed_queens.pop_back();

        previous_queen.row++;
        current_queen = previous_queen;
      }
    } else {
      if (current_queen.column + 1 == queens) {
        solutions++;

        #if DEBUG
          placed_queens.push_back(current_queen);

          board.assign(board.size(), 0);

          for (auto queen: placed_queens) {
            board[queen.row * queens + queen.column] = 1;
          }

          placed_queens.pop_back();

          print(board, queens);

          cout << endl;
        #endif

        current_queen.row++;

        while (current_queen.row == queens && current_queen.column > 0) {
          Queen previous_queen = placed_queens.back();
          placed_queens.pop_back();

          previous_queen.row++;
          current_queen = previous_queen;
        }
      } else {
        placed_queens.push_back(current_queen);
        current_queen = Queen(0, current_queen.column + 1);
      }
    }
  }

  return solutions;
}

int main(int argc, char **argv) {
  auto queens = 4;

  if (argc > 1) {
    queens = parse_ull(argv[1]);
  }

  cout << "Solving " << queens << " Queens Problem …" << endl;

  #if DEBUG
    auto board = vector<size_t>(queens * queens, 0);
    auto solutions = solve(board, queens);
  #else
    auto solutions = solve(queens);
  #endif

  cout << solutions << " solutions found.";

  #if DEBUG
    if (solutions > 0) {
      cout << " Here's the last one:" << endl;
    }

    print(board, queens);
  #else
    cout << endl;
  #endif

  return EXIT_SUCCESS;
}

