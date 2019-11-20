# Assignment 6

## Exercise 1

### Results for particle size 1000

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 1000       | 7926        |         |            |
| 2     | 1000       | 6213        | 1,28    | 0,64       |
| 4     | 1000       | 3795        | 2,09    | 0,52       |

### Results for particle size 2000

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 2000       | 31196       |         |            |
| 2     | 2000       | 23799       | 1,28    | 0,64       |
| 4     | 2000       | 13804       | 2,09    | 0,52       |

### Results for particle size 4000

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 2000       | 31196       |         |            |
| 2     | 2000       | 132759      | 1,28    | 0,64       |
| 4     | 2000       | 54969       | 2,09    | 0,52       |

![](chart.svg)

### Local Benchmarks

| slots | time steps | samples | runtime (s) |
| ----: | ---------: | ------: | ----------: |
|     1 |       1000 |    4000 |       32.69 |
|     2 |       1000 |    4000 |       24.59 |
|     4 |       1000 |    4000 |       14.43 |
|     8 |       1000 |    4000 |        8.03 |
|    16 |       1000 |    4000 |        6.79 |

