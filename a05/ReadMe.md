# Assignment 5

## Exercise 1

| Problem Size | Time Steps | Runtime |
|-------------:|-----------:|--------:|
|          100 |       1000 |    0.03 |
|          200 |       1000 |    0.10 |
|          500 |       1000 |    0.57 |
|         1000 |       1000 |    2.26 |
|         2000 |       1000 |    9.00 |
|         5000 |       1000 |   56.10 |
|        10000 |       1000 |  224.76 |
|        20000 |       1000 |  903.85 |

## Exercise 2

### What optimization methods can you come up with in order to improve the performance of Exercise 1?

Remove usage of `powf` in favour of simple multiplications and remove redundant calculations.

### What parallelization strategies would you consider for Exercise 1 and why?

For parallelizing exercise 1, our strategy will be to split the problem into chunks. For every particle, we need the next particle in order to calculate the velocity, so we only need to send two particles between two chunks.
