#include <iostream>
#include <vector>

#include "../../shared/parse_ull.hpp"

using namespace std;

int verify(const vector<vector<float>>& mat) {
  cout << "Verification: ";

  for (size_t i = 0; i < mat.size(); i++) {
    for (size_t j = 0; j < mat[0].size(); j++) {
      if (mat[i][j] != i * j) {

        cout << "FAILED" << endl;
        return EXIT_FAILURE;
      };
    }
  }

  cout << "OK" << endl;
  return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
  size_t n = 1000;

  if (argc > 1) {
    n = parse_ull(argv[1]);
  }

  cout << "Matrix Multiplication with n = " << n << endl;

  // Create a matrix depending on the row/column so we can verify it.
  auto mat_a = vector<vector<float>>(n, vector<float>(n));
  #pragma omp parallel for
  for (size_t i = 0; i < mat_a.size(); i++) {
    for (size_t j = 0; j < mat_a[0].size(); j++) {
      mat_a[i][j] = i * j;
    }
  }

 // Create an identity matrix.
  auto mat_b = vector<vector<float>>(mat_a[0].size(), vector<float>(mat_a[0].size()));
  #pragma omp parallel for
  for (size_t i = 0; i < mat_b.size(); i++) {
    for (size_t j = 0; j < mat_b[0].size(); j++) {
      mat_b[i][j] = i == j ? 1 : 0;
    }
  }

  auto mat_res = vector<vector<float>>(mat_a.size(), vector<float>(mat_b[0].size()));

  #pragma omp parallel for collapse(2)
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      float sum = 0;
      for (size_t k = 0; k < n; k++) {
        sum += mat_a[i][k] * mat_b[k][j];
      }
      mat_res[i][j] = sum;
    }
  }

  return verify(mat_res);
}
