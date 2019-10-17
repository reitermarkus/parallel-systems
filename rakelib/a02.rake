namespace :a02 do
  task :sync => :'lcc2:ping' do
    sync 'a02'
  end

  task :e01 => :sync do
    ssh <<~SH, directory: 'a02/e01'
      module load gcc/8.2.0
      module load openmpi/4.0.1

      make clean
      make
    SH

    qsub './pi_mpi.sh',
         parallel_environment: 'openmpi-8perhost',
         slots: 64,
         name: 'pi',
         directory: 'a02/e01'
  end

  task :e01_rust => :sync do
    ssh <<~SH, directory: 'a02/e01'
      set +u
      eval $(~/.linuxbrew/bin/brew shellenv)
      set -u

      source ~/.cargo/env
      cargo build --release
    SH

    qsub './pi_mpi_rust.sh',
         parallel_environment: 'openmpi-8perhost',
         slots: 64,
         name: 'pi',
         directory: 'a02/e01'
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
