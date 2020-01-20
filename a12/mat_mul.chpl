use LinearAlgebra;

config const size = 2552;
config const threads = 1;

var matA = Matrix(size, size, eltType=real);
var matB = eye(size, size);

forall i in 1..size do
  for j in 1..size do matA[i, j] = i * j;

var matRes = Matrix(size, size, eltType=real);
matB = transpose(matB);

const chunkSize = size / numLocales;

coforall loc in Locales {
  var chunkStart = chunkSize * loc.id;
  var chunkEnd = chunkStart + chunkSize;

  const threadChunkSize = chunkSize / threads;

  coforall t in 0..#threads {
    var start = threadChunkSize * t;
    var end = start + threadChunkSize;
    start += 1;
    if end > size then end = size;

    for i in start..end do
      for j in 1..size  do
        for k in vectorizeOnly(1..size) do
          matRes[i, j] += matA[i, k] * matB[j, k];
  }
}

var success = true;

for i in 1..size do
  for j in 1..size do
    if matRes[i, j] != i * j then
      success = false;

writeln("Verification: ", success);
