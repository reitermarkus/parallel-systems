#pragma once

#include <stdbool.h>
#include <stdlib.h>

typedef float value_t;

typedef value_t* Matrix;

Matrix create_matrix(size_t m, size_t n) {
  return (Matrix)calloc(m * n, sizeof(value_t));
}

void fill_matrices(Matrix a, Matrix b, size_t m, size_t n) {
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      a[i * m + j] = i * j;            // some matrix - note: flattened indexing!
      b[i * m + j] = (i == j) ? 1 : 0; // identity matrix
    }
  }
}

bool check(Matrix mat, size_t m, size_t n) {
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      if (mat[i * m + j] != i * j) return false;
    }
  }

  return true;
}

void print_matrix(Matrix mat, size_t m, size_t n) {
  for (size_t i = 0; i < m; i++) {
    for (size_t j = 0; j < n; j++) {
      printf("%f ", mat[i * n + j]);
    }
    printf("\n");
  }
}
