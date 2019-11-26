task :sync => :'lcc2:ping' do
  sync 'a07'
end

task :a07 => :sync do
  slots = 8
  ssh <<~SH, directory: 'a07'
    #{load_env :cpp}
    export OMP_NUM_THREADS=#{slots}
    make clean
    make THREADS=#{slots}
  SH

  qsub ompexec('./nbody_omp', env: :cpp),
       parallel_environment: 'openmp',
       slots: slots,
       name: 'nbody_omp',
       directory: 'a07'
end
