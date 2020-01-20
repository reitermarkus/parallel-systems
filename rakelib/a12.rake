task :sync => :'lcc2:ping' do
  sync 'a12'
end

task :a12_bench_pi => :sync do
  ssh <<~SH, directory: 'a12'
    #{load_env :chapel}
    make clean
    make -j pi
  SH

  [1, 2, 4, 8].each { |n|
    qsub chplexec('./pi', threads: n), "--threads=#{n}",
        parallel_environment: 'openmp',
        slots: n,
        name: 'pi',
        directory: 'a12'
  }
end

task :a12_bench_mat_mul => :sync do
  ssh <<~SH, directory: 'a12'
    #{load_env :chapel}
    make clean
    make -j mat_mul
  SH

  [1, 2, 4, 8].each { |n|
    qsub chplexec('./mat_mul', threads: n),
        parallel_environment: 'openmp',
        slots: n,
        name: 'mat_mul',
        directory: 'a12'
  }
end

task :a12, [:n] => :sync do |n:|
  ssh <<~SH, directory: 'a12'
    #{load_env :cpp}
    make clean
    make -j
  SH

  qsub ompexec('./pi', threads: n, env: :chapel),
      parallel_environment: 'openmp',
      slots: n,
      name: 'pi',
      directory: 'a12'
end
