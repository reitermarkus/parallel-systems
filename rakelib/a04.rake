namespace :a04 do
  task :sync => :'lcc2:ping' do
    sync 'a04'
  end

  task :e01 => :sync do
    ssh <<~SH, directory: 'a04/e01'
      #{load_env :cpp}
      make clean
      make -j 8
    SH

    qsub mpiexec('./heat_stencil_3d_seq', env: :cpp),
         parallel_environment: 'openmpi-1perhost',
         slots: 1,
         name: 'heat_stencil_3d_cube',
         directory: 'a04/e01'
  end

  task :e01_docker => :'docker:copy_exercises' do
    cd './docker' do
      sh 'sh', './cluster.sh', 'exec', 'make -C ./a04/e01/'
      sh 'sh', './cluster.sh', 'exec', 'time ./a04/e01/heat_stencil_3d_slab_mpi'
    end
  end
end