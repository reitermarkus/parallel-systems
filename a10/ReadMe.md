# Assignment 10

## Exercise 1

| Optimization | Runtime |
|--------------|---------|
| None         | 8.357 s |
| Only OpenMP  | 1.990 s |
| Only MPI     | 2.400 s |
| MPI & OpenMP | 1.765 s |

# MPI Optimization

For the MPI optimization, we changed our topology from 2D to 1D. Before the optimization, we split the rows and columns into equal chunks. Now we are splitting only the rows into slabs.

# OpenMP Optimization

For the OpenMP optimization, we used the `simd` pragma. We split the inner loop of the calculation into two loops in which the inner loop is vectorized. Additionally, we skipped the check for the heat source inside of the loop and instead reset the heat source after the loop.
