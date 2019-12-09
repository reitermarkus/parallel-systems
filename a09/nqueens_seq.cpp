#include <vector>

#include "../shared/parse_ull.hpp"

#include "nqueens.hpp"

bool check_vecinity(const vector<size_t>& board, int row, int col, int queens) {
  for (int i = 0; i < col; i++)
    if (board[row * queens + i])
      return false;

  for (int i = row, j = col; i >= 0 && j >= 0; i--, j--)
    if (board[i * queens + j])
      return false;

  for (int i = row, j = col; j >= 0 && i < queens; i++, j--)
    if (board[i * queens + j])
      return false;

  return true;
}

bool solve(vector<size_t>& board, int col, int queens) {
  if (col >= queens)
    return true;

  for (int i = 0; i < queens; i++) {
    if (check_vecinity(board, i, col, queens)) {

      board[i * queens + col] = 1;

      if (solve(board, col + 1, queens))
        return true;

      board[i * queens + col] = 0;
    }
  }

  return false;
}

int main(int argc, char **argv) {
  auto queens = 4;

  if (argc > 1) {
    queens = parse_ull(argv[1]);
  }

  auto board = vector<size_t>(queens * queens, 0);

  if (!solve(board, 0, queens)) {
    cout << "Solution does not exist" << endl;
    return EXIT_FAILURE;
  }

  print(board, queens);

  return EXIT_SUCCESS;
}

