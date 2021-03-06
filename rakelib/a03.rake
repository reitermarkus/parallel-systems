namespace :a03 do
  task :sync => :'lcc2:ping' do
    sync 'a03'
  end

  task :e01 => :sync do
    ssh <<~SH, directory: 'a03/e01'
      #{load_env :cpp}
      make clean
      make -j 8
    SH

    qsub mpiexec('./heat_stencil_2d_mpi', env: :cpp),
         parallel_environment: 'openmpi-8perhost',
         slots: 16,
         name: 'heat_stencil_2d',
         directory: 'a03/e01'
  end

  task :e02 => :sync do
    ssh <<~SH, directory: 'a03/e02'
      #{load_env :cpp}
      make clean
      make -j 8
    SH

    qsub mpiexec('./heat_stencil_3d_mpi', env: :cpp),
         parallel_environment: 'openmpi-8perhost',
         slots: 8,
         name: 'heat_stencil_3d',
         directory: 'a03/e02'
  end

  task :e01_docker => :'docker:copy_exercises' do
    cd './docker' do
      sh 'sh', './cluster.sh', 'exec', 'make -C ./a03/e01/'
      sh 'sh', './cluster.sh', 'exec', 'time ./a03/e01/heat_stencil_2d_seq'
    end
  end

  task :e02_docker => :'docker:copy_exercises' do
    cd './docker' do
      sh 'sh', './cluster.sh', 'exec', 'make -C ./a03/e02/'
      sh 'sh', './cluster.sh', 'exec', 'time ./a03/e02/heat_stencil_3d_seq'
    end
  end
end
