# Assignment 1

## Exercise 1

To run this exercise, execute `rake a02:e01`.

For our parallelization strategy we chose to split the number of samples into equal chunks and then reduce the partial results by summing them up. We chose this strategy because this seems like an embarrassingly parallel problem.

### Results for 8 slots

| Slots | Samples       | Result     | Time  |
|-------|---------------|------------|-------|
| 8     |   500.000.000 | 3.14168512 | 4579  |
| 8     | 1.000.000.000 | 3.1415672  | 8901  |
| 8     | 2.000.000.000 | 3.14155388 | 17838 |

### Results for 16 slots

| Slots | Samples       | Result     | Time |
|-------|---------------|------------|------|
| 16    |   500.000.000 | 3.14163763 | 2702 |
| 16    | 1.000.000.000 | 3.14160268 | 5295 |
| 16    | 2.000.000.000 | 3.14159683 | 8919 |

### Results for 32 slots

| Slots | Samples       | Result     | Time |
|-------|---------------|------------|------|
| 32    |   500.000.000 | 3.14150258 | 1354 |
| 32    | 1.000.000.000 | 3.14156726 | 2443 |
| 32    | 2.000.000.000 | 3.14157818 | 4585 |

Our parallelization strategy implies that the total number of samples can fit in an unsigned 64-bit integer, anything larger is not supported.

## Exercise 2

To run this exercise, execute `rake a02:e02`.

For our parallelization strategy we chose to split the room into equal chunks. For each time step, we then needed to send the first and last element to the left and right neighbor rank, respectively.

Our parallelization strategy has the effect that for a small room size, the runtime actually increases when using more processes due to the send/receive overhead.
