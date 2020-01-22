task :sync => :'lcc2:ping' do
  sync 'a12'
end

task :a12_pi, [:n] => :sync do |n:|
  ssh <<~SH, directory: 'a12'
  #{load_env :chapel}
    make clean
    make -j pi
  SH

  qsub chplexec('./pi', threads: n, env: :chapel), "--threads=#{n}",
  parallel_environment: 'openmp',
  slots: n,
  name: 'pi',
  directory: 'a12'
end

task :a12_bench_pi => :sync do
  ssh <<~SH, directory: 'a12'
    #{load_env :chapel}
    make clean
    make -j pi
  SH

  [1, 2, 4, 8].each { |n|
    qsub chplexec('./pi', threads: n, env: :chapel), "--threads=#{n}",
        parallel_environment: 'openmp',
        slots: n,
        name: 'pi',
        directory: 'a12'
  }
end

task :a12_bench_pi_dist => :sync do
  ssh <<~SH, directory: 'a12'
    #{load_env :chapel_mpi}
    make clean
    make -j pi
  SH

  [1, 2, 4].each { |n|
    qsub chplexec('./pi', threads: 8, env: :chapel_mpi), "--threads=8", "-nl", n,
        parallel_environment: 'openmpi-1perhost',
        slots: n,
        output_file: "output_#{n}.log",
        error_log: "error_#{n}.log",
        runtime: '00:05:00',
        name: 'pi_dist',
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
    qsub chplexec('./mat_mul', threads: n, env: :chapel), "--threads=#{n}",
        parallel_environment: 'openmp',
        slots: n,
        name: 'mat_mul',
        directory: 'a12'
  }
end

task :a12_bench_mat_mul_dist => :sync do
  ssh <<~SH, directory: 'a12'
    #{load_env :chapel_mpi}
    make clean
    make -j mat_mul
  SH

  [1, 2, 4].each { |n|
    qsub chplexec('./mat_mul', threads: 8, env: :chapel_mpi), "--threads=8", "-nl", n,
        parallel_environment: 'openmpi-1perhost',
        slots: n,
        output_file: "output_#{n}.log",
        error_log: "error_#{n}.log",
        runtime: '00:05:00',
        name: 'mat_mul_dist',
        directory: 'a12'
  }
end
