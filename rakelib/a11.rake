task :sync => :'lcc2:ping' do
  sync 'a11'
end

task :a11_bench => :sync do
  ssh <<~SH, directory: 'a11'
    #{load_env :cpp}
    make clean
    make -j
  SH

  qsub './real_seq',
        parallel_environment: 'openmp',
        slots: 1,
        name: 'real',
        directory: 'a11'

  [1, 2, 4, 8].each { |n|
    qsub ompexec('./real_omp', threads: n, env: :cpp),
        parallel_environment: 'openmp',
        slots: n,
        name: 'real_omp',
        directory: 'a11'
  }
end

task :a11, [:n] => :sync do |n:|
  ssh <<~SH, directory: 'a11'
    #{load_env :cpp}
    make clean
    make -j
  SH

  qsub ompexec('./real_omp', threads: n, env: :cpp),
      parallel_environment: 'openmp',
      slots: n,
      name: 'real_omp',
      directory: 'a11'
end
