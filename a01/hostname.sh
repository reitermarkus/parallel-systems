#!/usr/bin/env bash

set -euo pipefail

module load openmpi/4.0.1

mpiexec hostname
