# Assignment 4

## Exercise 1 - heat stencil 3D cube

### Results for room size 50

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 200        | 15133       |         |            |
| 2     | 200        | 22272       | 0,68    | 0,34       |
| 4     | 200        | 23000       | 0,66    | 0,17       |
| 8     | 200        | 3881        | 3,90    | 0,49       |
| 16    | 200        | 4619        | 3,28    | 0,21       |

### Results for room size 64

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 100        | 19928       |         |            |
| 2     | 100        | 29527       | 0,65    | 0,33       |
| 4     | 100        | 29475       | 0,68    | 0,17       |
| 8     | 100        | 5012        | 3,98    | 0,50       |
| 16    | 100        | 5256        | 3,79    | 0,24       |

### Results for room size 96

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 100        | 140389      |         |            |
| 8     | 100        | 21459       | 6,54    | 0,82       |
| 16    | 100        | 22225       | 6.32    | 0,40       |

### Results for room size 128

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 100        | 298955      |         |            |
| 8     | 100        | 65054       | 4,60    | 0,58       |
| 16    | 100        | 65352       | 4.57    | 0,29       |

## Exercise 1 - heat stencil 3D slabs

### Results for room size 50

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 200        | 13177       |         |            |
| 2     | 200        | 9569        | 1,37    | 0,69       |
| 4     | 200        | 6031        | 2,18    | 0,55       |
| 8     | 200        | 4055        | 3,24    | 0,41       |

### Results for room size 96

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 100        | 130029      |         |            |
| 4     | 100        | 32664       | 3,98    | 0,99       |
| 8     | 100        | 19061       | 6,82    | 0,85       |
| 16    | 100        | 15981       | 8,14    | 0,51       |
| 32    | 100        | 13029       | 9,98    | 0,31       |

## Exercise 1 - heat stencil 3D poles

### Results for room size 50

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 200        | 13177       |         |            |
| 2     | 200        | 18098       | 0,73    | 0,36       |
| 4     | 200        | 5720        | 2,30    | 0,58       |
| 8     | 200        | 5764        | 2,29    | 0,29       |
| 16    | 200        | 4566        | 3,24    | 0,41       |

### Results for room size 96

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 100        | 130029      |         |            |
| 4     | 100        | 32804       | 3,96    | 0,99       |
| 8     | 100        | 32669       | 3,98    | 0,50       |
| 16    | 100        | 16200       | 8,03    | 0,50       |
| 32    | 100        | 13178       | 9,87    | 0,31       |

![](images/speedup-room-size-96.svg)
![](images/speedup-room-size-50.svg)
![](images/efficiency-room-size-96.svg)
![](images/efficiency-room-size-50.svg)

# Local Benchmarks

| Variant            | Slots | Time  | Problem Size | Time Steps Factor | Notes    |
|--------------------|-------|-------|--------------|-------------------|----------|
| slab blocking      |     8 | 53,78 | 100          | 500               |          |
| slab blocking      |    16 | 66,21 | 100          | 500               | segfault |
| slab non-blocking  |     8 | 44,75 | 100          | 500               |          |
| slab non-blocking  |    16 | 51,56 | 100          | 500               | segfault |
| cube blocking      |     8 | 47,38 | 100          | 500               |          |
| cube blocking      |    16 | 56,00 | 100          | 500               |          |
| cube non-blocking  |     8 | 43,62 | 100          | 500               |          |
| cube non-blocking  |    16 | 48,82 | 100          | 500               |          |
| pole blocking      |     8 | 71,22 | 100          | 500               |          |
| pole blocking      |    16 | 48,25 | 100          | 500               |          |
| pole non-blocking  |     8 | 70,72 | 100          | 500               |          |
| pole non-blocking  |    16 | 39,51 | 100          | 500               |          |

![](images/blocking-vs-nonblocking.svg)
