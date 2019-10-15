#!/usr/bin/env bash

set -euo pipefail

module load openmpi/4.0.1

export PATH="/scratch/c703429/osu-benchmark/libexec/osu-micro-benchmarks/mpi/pt2pt:${PATH}"

mpiexec --display-map --display-allocation -n 2 "$1" osu_latency
mpiexec --display-map --display-allocation -n 2 "$1" osu_bw
