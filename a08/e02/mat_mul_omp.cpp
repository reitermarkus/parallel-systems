#include <stdio.h>
#include "matrix.h"

#include "../../shared/parse_ull.hpp"

int main(int argc, char** argv) {
  size_t n = 1000;

  if (argc > 1) {
    n = parse_ull(argv[1]);
  }

  printf("Matrix Multiplication with n=%zu\n", n);

  // -------------------- SETUP -------------------- //
  Matrix mtx_a = create_matrix(n, n);
  Matrix mtx_b = create_matrix(n, n);

  fill_matrices(mtx_a, mtx_b, n, n);

  Matrix mtx_res = create_matrix(n, n);

  // ------------------- COMPUTE -------------------- //
  #pragma omp parallel for
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      value_t sum = 0;
      for (size_t k = 0; k < n; k++) {
        sum += mtx_a[i * n + k] * mtx_b[k * n + j];
      }
      mtx_res[i * n + j] = sum;
    }
  }

  // ------------------- CHECK ------------------- //
  bool success = check(mtx_res, n, n);
  printf("Verification: %s\n", success ? "OK" : "FAILED");

  // ----------------- CLEAN UP ------------------ //
  free(mtx_a);
  free(mtx_b);
  free(mtx_res);

  return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}
