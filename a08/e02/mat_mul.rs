fn matrix_multiplication(mat_a : Vec<f32>, mat_b: Vec<f32>, n: usize) -> Vec<f32> {
  let mut mat_res = vec![0.0; n * n];

  for i in 0..n {
    for j in 0..n {
      mat_res[i * n + j] = (0..n).fold(0.0, |acc, k| acc + mat_a[i * n + k] * mat_b[k * n + j]);
    }
  }

  mat_res
}

fn matrix_verification(matrix: &Vec<f32>, n: usize) -> bool {
  for i in 0..n {
    for j in 0..n {
      if matrix[i * n + j] != i as f32 * j as f32 {
        return false;
      }
    }
  }

  true
}

fn fill_matrices(mat_a : &mut Vec<f32>, mat_b: &mut Vec<f32>, n: usize) {
  for i in 0..n {
    for j in 0..n {
      mat_a[i * n + j] = i as f32 * j as f32;            // some matrix - note: flattened indexing!
      mat_b[i * n + j] = if i == j { 1.0 } else { 0.0 } // identity matrix
    }
  }
}

fn main() {
  let n = 500;
  let mut mat_a = vec![0.0; n * n];
  let mut mat_b = vec![0.0; n * n];

  fill_matrices(&mut mat_a, &mut mat_b, n);
  let mat_res = matrix_multiplication(mat_a, mat_b, n);

  if matrix_verification(&mat_res, n) == false {
    panic!("verification failed");
  }
}
