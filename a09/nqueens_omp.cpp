#include <vector>
#include <omp.h>

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

bool solve(vector<size_t>& board, size_t queens) {
  vector<Queen> global_placed_queens;

  bool searching = true;

  #pragma omp parallel
  {
    vector<Queen> placed_queens;
    placed_queens.reserve(queens);

    auto current_queen = Queen(omp_get_thread_num(), 0);

    while (searching && current_queen.row < queens && current_queen.column < queens) {
      if (current_queen.can_be_killed_by(placed_queens)) {
        current_queen.row++;

        while (current_queen.row == queens && current_queen.column > 0) {
          Queen previous_queen = placed_queens.back();
          placed_queens.pop_back();

          if (previous_queen.column == 0) {
            previous_queen.row += omp_get_num_threads();
          } else {
            previous_queen.row++;
          }

          current_queen = previous_queen;
        }
      } else {
        placed_queens.push_back(current_queen);
        current_queen = Queen(0, current_queen.column + 1);
      }
    }

    #pragma omp critical
    if (placed_queens.size() == queens) {
      searching = false;
      global_placed_queens = placed_queens;
    }
  }

  for (auto queen: global_placed_queens) {
    board[queen.row * queens + queen.column] = 1;
  }

  return global_placed_queens.size() == queens;
}

int main(int argc, char **argv) {
  auto queens = 4;

  if (argc > 1) {
    queens = parse_ull(argv[1]);
  }

  cout << "Solving " << queens << " Queens Problem …" << endl;

  auto board = vector<size_t>(queens * queens, 0);

  if (!solve(board, queens)) {
    cout << "Solution does not exist" << endl;
    return EXIT_FAILURE;
  }

  print(board, queens);

  return EXIT_SUCCESS;
}

