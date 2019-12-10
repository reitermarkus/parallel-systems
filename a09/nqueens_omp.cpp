#include <vector>
#include <omp.h>

#include "../shared/parse_ull.hpp"

#include "nqueens.hpp"

size_t solve(size_t queens) {
  vector<Queen> global_placed_queens;

  size_t solutions = 0;

  #pragma omp parallel
  {
    vector<Queen> placed_queens;
    placed_queens.reserve(queens);

    auto current_queen = Queen(omp_get_thread_num(), 0);

    while (current_queen.row < queens && current_queen.column < queens) {
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
        if (current_queen.column + 1 == queens) {
          #pragma omp atomic
          solutions++;

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

  auto solutions = solve(queens);
  cout << solutions << " solutions found." << endl;

  return EXIT_SUCCESS;
}

