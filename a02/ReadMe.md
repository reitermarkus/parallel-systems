# Assignment 1

## Exercise 1

To run this exercise, execute `rake a02:e01`.

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

Our parallelization strategy implies that the total number of samples can fit in an unsigned 64-bit integer, anything larger is not supported.

## Exercise 2

To run this exercise, execute `rake a02:e02`.

