# Assignment 1

## Exercise 1

To run this exercise, execute `rake a01:e01`.

In my opinion, the five most important parameters when submitting a job are:

- `-pe` because it is needed to set up the parallel environment
- `-o` to customize the output file name
- `-e` to log errors for debugging into a separate file
- `-cwd` so the output/log files are written in the current directory
- `-N` to easily check the status using `qstat -j`

To run the program in parallel, we need to set the `-pe` to `openmpi-*perhost` and use `mpiexec` to execute is.

## Exercise 2

To run this exercise, execute `rake a01:e02`.

###  Latency Test

| Size    | Same Socket, Different Cores | Same Node, Different Sockets | Different Nodes |
|--------:|-----------------------------:|-----------------------------:|----------------:|
|       0 |                         0.41 |                         0.63 |            3.50 |
|       1 |                         0.45 |                         0.68 |            3.55 |
|       2 |                         0.45 |                         0.68 |            3.55 |
|       4 |                         0.45 |                         0.68 |            3.56 |
|       8 |                         0.46 |                         0.68 |            3.62 |
|      16 |                         0.47 |                         0.82 |            3.63 |
|      32 |                         0.48 |                         0.82 |            3.70 |
|      64 |                         0.52 |                         0.92 |            3.84 |
|     128 |                         0.55 |                         1.16 |            4.85 |
|     256 |                         0.59 |                         1.41 |            5.34 |
|     512 |                         0.87 |                         2.09 |            6.18 |
|    1024 |                         1.01 |                         2.90 |            7.31 |
|    2048 |                         1.31 |                         4.70 |            9.90 |
|    4096 |                         3.98 |                         4.83 |           12.58 |
|    8192 |                         4.88 |                         5.83 |           18.44 |
|   16384 |                         6.65 |                         7.64 |           26.28 |
|   32768 |                         9.25 |                        10.26 |           36.69 |
|   65536 |                        14.53 |                        15.57 |           57.71 |
|  131072 |                        25.14 |                        26.70 |          100.59 |
|  262144 |                        47.07 |                        48.25 |          187.17 |
|  524288 |                        91.97 |                        92.27 |          358.09 |
| 1048576 |                       182.22 |                       181.56 |          699.05 |
| 2097152 |                      1292.87 |                       368.71 |         1379.32 |
| 4194304 |                      2893.50 |                      2589.14 |         2739.89 |

![](images/latency.svg)

### Bandwidth Test

| Size    | Same Socket, Different Cores | Same Node, Different Sockets | Different Nodes |
|--------:|-----------------------------:|-----------------------------:|----------------:|
|       1 |                         4.18 |                         4.50 |            0.63 |
|       2 |                         8.41 |                         9.12 |            1.26 |
|       4 |                        16.93 |                        18.44 |            2.51 |
|       8 |                        33.77 |                        36.31 |            5.09 |
|      16 |                        55.31 |                        67.83 |           10.22 |
|      32 |                       120.30 |                       136.57 |           20.47 |
|      64 |                       204.32 |                       201.30 |           40.60 |
|     128 |                       356.96 |                       193.72 |           78.03 |
|     256 |                       605.64 |                       294.01 |          153.61 |
|     512 |                      1338.97 |                       470.83 |          294.81 |
|    1024 |                      2463.11 |                       641.71 |          533.48 |
|    2048 |                      4093.77 |                       816.20 |          752.59 |
|    4096 |                      1555.18 |                      1221.09 |          926.20 |
|    8192 |                      2423.38 |                      1988.39 |         1064.08 |
|   16384 |                      3105.18 |                      2743.91 |         1099.46 |
|   32768 |                      4150.19 |                      3807.54 |         1305.46 |
|   65536 |                      4874.76 |                      4717.54 |         1422.41 |
|  131072 |                      5549.42 |                      5394.80 |         1483.04 |
|  262144 |                      5921.42 |                      5820.29 |         1502.15 |
|  524288 |                      6124.61 |                      6071.48 |         1519.23 |
| 1048576 |                      5966.19 |                      6186.82 |         1529.09 |
| 2097152 |                      5707.71 |                      5901.68 |         1535.17 |
| 4194304 |                      1629.33 |                      1641.86 |         1537.56 |

![](images/bandwith.svg)

To verify the rank placement, we can pass the `--display-map`, `--display-allocation` to `mpiexec` which displays information about the node/socket/core a given process is running on.

The results are somewhat stable, they are whithin ± 10% of the reference results in the tables above.
