use LinearAlgebra;

config const size = 2552;

var matA = Matrix(size, size, eltType=real);
var matB = eye(size, size);

forall i in 1..size do
  for j in 1..size do
    matA[i, j] = i * j;

var matRes = matA.dot(matB);

var success = true;

for i in 1..size do
  for j in 1..size do
    if matRes[i, j] != i * j then
      success = false;

writeln("Verification: ", success);
