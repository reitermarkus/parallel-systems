namespace :a02 do
  task :sync => :'lcc2:ping' do
    sync 'a02'
  end

  task :e01 => :sync do
    ssh <<~SH, directory: 'a02/e01'
      #{load_env :cpp}
      make clean
      make
    SH

    qsub mpiexec('./pi_mpi', env: :cpp),
         parallel_environment: 'openmpi-8perhost',
         slots: 64,
         name: 'pi',
         directory: 'a02/e01'
  end

  task :e01_rust => :sync do
    ssh <<~SH, directory: 'a02/e01'
      #{load_env :rust}
      cargo build --release
    SH

    qsub mpiexec('./target/release/pi_mpi', env: :rust),
         parallel_environment: 'openmpi-8perhost',
         slots: 64,
         name: 'pi',
         directory: 'a02/e01'
  end

  task :e01_docker => :'docker:copy_exercises' do
    cd './docker' do
        sh 'sh', './cluster.sh', 'exec', 'make -C ./a02/e01/'
        sh 'sh', './cluster.sh', 'exec', 'mpiexec', '-n', '8', './a02/e01/pi_mpi'
    end
  end

  task :e02 => :sync do
    ssh <<~SH, directory: 'a02/e02'
      module load gcc/8.2.0
      module load openmpi/4.0.1
      make clean
      make run
    SH
  end
end
