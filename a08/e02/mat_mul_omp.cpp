#include <iostream>
#include <vector>

#include "../../shared/parse_ull.hpp"

using namespace std;

int verify(const vector<float>& mat, size_t n) {
  cout << "Verification: ";

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      if (mat[i * n + j] != i * j) {

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
  auto mat_a = vector<float>(n * n);
  #pragma omp parallel for
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      mat_a[i * n + j] = i * j;
    }
  }

 // Create an identity matrix.
  auto mat_b = vector<float>(n * n);
  #pragma omp parallel for
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      mat_b[i * n + j] = i == j ? 1 : 0;
    }
  }

  auto mat_res = vector<float>(n * n);

  #pragma omp parallel for collapse(2)
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      float sum = 0;
      for (size_t k = 0; k < n; k++) {
        sum += mat_a[i * n + k] * mat_b[k * n + j];
      }
      mat_res[i * n + j] = sum;
    }
  }

  return verify(mat_res, n);
}
