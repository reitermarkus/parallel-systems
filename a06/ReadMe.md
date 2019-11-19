# Assignment 5

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

### Results for particle size 2000

| slots | time steps | result (ms) | speedup | efficiency |
|-------|------------|-------------| ------- | ---------- |
| 1     | 2000       | 31196       |         |            |
| 2     | 2000       | 132759      | 1,28    | 0,64       |
| 4     | 2000       | 54969       | 2,09    | 0,52       |


![](chart.svg)

## Exercise 2

### What optimization methods can you come up with in order to improve the performance of Exercise 1?

Remove usage of `powf` in favour of simple multiplications. Also remove redundant calculations. In the sense that we could change `force = G * (mass_1 * mass_2) / radius^2` to `force = G * (mass_1 * mass_2) / radius^2 * dt`, so we do not have to multiply `dt` and then divide by the mass of the particle and instead only have to multiply the force with the mass for the velocity of each particle.

### What parallelization strategies would you consider for Exercise 1 and why?

For parallelizing exercise 1, our strategy will be to split the problem into chunks of particles and distribute those among the ranks. For every particle, we need the next particle in order to calculate the velocity, so we only need to send two particles between two chunks and can calculate the velocity and position of the other particles pairwise independently.

With this method the communication overhead should not be too bad, while still maintaining good distribution of work among the ranks.
