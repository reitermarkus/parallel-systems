eval $(~/.linuxbrew/bin/brew shellenv)

mpiexec -n "$NSLOTS" --mca btl tcp,self ./target/release/pi_mpi
