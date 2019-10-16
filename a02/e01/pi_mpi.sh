module load gcc/8.2.0
module load openmpi/4.0.1

mpiexec -n "$NSLOTS" --mca btl tcp,self ./pi_mpi
