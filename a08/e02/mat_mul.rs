use std::{env, iter::Sum, ops::*};

struct Matrix<T> {
  rows: usize,
  columns: usize,
  data: Vec<T>,
}

impl<T> Matrix<T> where T: Default + Clone {
  #[inline]
  pub fn new(rows: usize, columns: usize) -> Self {
    Self {
      rows, columns, data: vec![T::default(); rows * columns],
    }
  }
}

impl<T> Matrix<T> {
  #[inline]
  pub unsafe fn get_unchecked(&self, i: usize, j: usize) -> &T {
    self.data.get_unchecked(i * self.rows + j)
  }
}

impl Matrix<f32> {
  pub fn i_times_j(rows: usize, columns: usize) -> Self {
    let mut m = Self::new(rows, columns);

    for i in 0..rows {
      for j in 0..columns {
        m[(i, j)] = (i * j) as f32;
      }
    }

    m
  }

  pub fn identity(n: usize) -> Self {
    let mut m = Self::new(n, n);

    for ij in 0..n {
      m[(ij, ij)] = 1.0;
    }

    m
  }
}

impl<T> Index<(usize, usize)> for Matrix<T> {
  type Output = T;

  fn index(&self, i: (usize, usize)) -> &Self::Output {
    assert!(i.0 < self.rows);
    assert!(i.1 < self.columns);
    &self.data[i.0 * self.rows + i.1]
  }
}

impl<T> IndexMut<(usize, usize)> for Matrix<T> {
  fn index_mut(&mut self, i: (usize, usize)) -> &mut Self::Output {
    assert!(i.0 < self.rows);
    assert!(i.1 < self.columns);
    &mut self.data[i.0 * self.rows + i.1]
  }
}

impl<'mul, T, O> Mul for &'mul Matrix<T>
where
  T: 'mul,
  T: Sync,
  O: Send,
  &'mul T: Mul<Output = O>,
  O: Sum,
{
  type Output = Matrix<O>;

  fn mul(self, rhs: Self) -> Self::Output {
    let lhs = self;

    let new_rows = lhs.rows;
    let new_columns = rhs.columns;
    assert!(lhs.columns == rhs.rows);
    let shared_edge = lhs.columns;

    use rayon::prelude::*;

    let data = (0..new_rows).into_par_iter().flat_map(move |i| {
      (0..new_columns).into_par_iter().map(move |j| {
        (0..shared_edge).map(|k| unsafe { lhs.get_unchecked(i, k) * rhs.get_unchecked(k, j) }).sum()
      })
    }).collect();

    Matrix {
      rows: new_rows,
      columns: new_columns,
      data,
    }
  }
}

fn verify(matrix: &Matrix<f32>) -> bool {
  for i in 0..matrix.rows {
    for j in 0..matrix.columns {
      if matrix[(i, j)] != i as f32 * j as f32 {
        return false;
      }
    }
  }

  true
}

fn main() {
  let n = env::args().nth(1)
    .map(|n| n.parse().expect("Failed to parse argument"))
    .unwrap_or(1_000);

  let mat_a = Matrix::<f32>::i_times_j(n, n);
  let mat_b = Matrix::<f32>::identity(n);

  let mat_res = &mat_a * &mat_b;

  if verify(&mat_res) == false {
    panic!("verification failed");
  }
}
