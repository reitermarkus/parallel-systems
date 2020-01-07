task :sync => :'lcc2:ping' do
  sync 'a10'
end

task :a10, [:mpi_optimization, :openmp_optimization, :problem_size] => :sync do |task, mpi_optimization:, openmp_optimization:, problem_size:|
  problem_size ||= 100

  ssh <<~SH, directory: 'a10'
    #{load_env :cpp}
    make clean
    make MPI_OPTIMIZATION=#{mpi_optimization || 0} OPENMP_OPTIMIZATION=#{openmp_optimization || 0}
  SH

  qsub mpi_ompexec('./heat_stencil_2d_mpi_omp', threads: 8, env: :cpp), problem_size.to_s,
       parallel_environment: 'openmpi-1perhost',
       slots: 4,
       name: 'heat_stencil_2d_mpi_omp',
       directory: 'a10'
end

task :a10_bench => :sync do
  (0...3).each { |n|
    [200, 400].each{ |ps|
      [[0, 0], [0, 1], [1, 0], [1, 1]].each { |(mpi, omp)|
        ssh <<~SH, directory: 'a10'
          #{load_env :cpp}
          make clean
          make MPI_OPTIMIZATION=#{mpi} OPENMP_OPTIMIZATION=#{omp}
        SH

        qsub mpi_ompexec('./heat_stencil_2d_mpi_omp', threads: 8, env: :cpp), ps.to_s,
            parallel_environment: 'openmpi-1perhost',
            slots: 2 ** n,
            output_file: "output_#{n}_#{ps}_#{mpi}_#{omp}.log",
            error_log: "error_#{n}_#{ps}_#{mpi}_#{omp}.log",
            name: 'heat_stencil_2d_mpi_omp',
            directory: 'a10'
      }
    }
  }
end
