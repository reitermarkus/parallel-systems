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

# 1 Node / 8 Threads

| MPI Optimization | OpenMP Optimization | Problem Size | Runtime (ms) |
|------------------|---------------------|--------------|--------------|
| 0                | 0                   | 200          |        13780 |
| 0                | 1                   | 200          |         3795 |
| 1                | 0                   | 200          |         3902 |
| 1                | 1                   | 200          |         3419 |
| 0                | 0                   | 400          |        51563 |
| 0                | 1                   | 400          |        10912 |
| 1                | 0                   | 400          |        11149 |
| 1                | 1                   | 400          |         9971 |

![](1node_8threads.svg)

# 2 Nodes / 8 Threads

| MPI Optimization | OpenMP Optimization | Problem Size | Runtime (ms) |
|------------------|---------------------|--------------|--------------|
| 0                | 0                   | 200          |        14387 |
| 0                | 1                   | 200          |         4071 |
| 1                | 0                   | 200          |        10857 |
| 1                | 1                   | 200          |        11049 |
| 0                | 0                   | 400          |        52142 |
| 0                | 1                   | 400          |        11459 |
| 1                | 0                   | 400          |        16611 |
| 1                | 1                   | 400          |        15893 |

![](2nodes_8threads.svg)