use LinearAlgebra;
use Time;

config const size = 2552;
config const threads = 1;

var matA = Matrix(size, size, eltType=real);
var matB = eye(size, size);

var t: Timer;
t.start();

forall i in 1..size do
  for j in 1..size do matA[i, j] = i * j;

var matRes = Matrix(size, size, eltType=real);
matB = transpose(matB);

forall i in 1..size do
  for j in 1..size  do
    for k in vectorizeOnly(1..size) do
      matRes[i, j] += matA[i, k] * matB[j, k];

t.stop();

var success = true;

for i in 1..size do
  for j in 1..size do
    if matRes[i, j] != i * j then
      success = false;

writeln("Verification: ", success);

writeln("Runtime: ", t.elapsed());
