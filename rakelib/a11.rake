task :sync => :'lcc2:ping' do
  sync 'a11'
end

task :a11 => :sync do
  ssh <<~SH, directory: 'a11'
    #{load_env :cpp}
    make clean
    make -j 8
  SH

  qsub mpiexec('./real_seq', env: :cpp),
        parallel_environment: 'openmpi-1perhost',
        slots: 1,
        name: 'real',
        directory: 'a11'

  [1, 2, 4, 8].each { |n|
    ssh <<~SH, directory: 'a11'
      #{load_env :cpp}
      make clean
      make THREADS=#{n}
    SH

    qsub ompexec('./real_omp', threads: n, env: :cpp), nil,
        parallel_environment: 'openmp',
        slots: n,
        name: 'real_omp',
        directory: 'a11'
  }
end
