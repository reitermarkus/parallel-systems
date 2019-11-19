namespace :a06 do
  task :sync => :'lcc2:ping' do
    sync 'a06'
  end

  task :e01 => :sync do
    ssh <<~SH, directory: 'a06/e01'
      #{load_env :cpp}
      make clean
      make -j 8
    SH

    qsub mpiexec('./nbody_mpi', env: :cpp),
         parallel_environment: 'openmpi-4perhost',
         slots: 4,
         name: 'nbody_mpi',
         directory: 'a06/e01'
  end
end
