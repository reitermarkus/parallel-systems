namespace :a05 do
  task :sync => :'lcc2:ping' do
    sync 'a05'
  end

  task :e01 => :sync do
    ssh <<~SH, directory: 'a05/e01'
      #{load_env :cpp}
      make clean
      make -j 8
    SH

    qsub mpiexec('./nbody_seq', env: :cpp),
         parallel_environment: 'openmpi-1perhost',
         slots: 1,
         name: 'nbody_seq',
         directory: 'a05/e01'
  end
end
