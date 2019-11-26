task :sync => :'lcc2:ping' do
  sync 'a07'
end

task :a07, [:slots, :problem_size] => :sync do |task, slots:, problem_size:|
  slots ||= 8
  problem_size ||= 2000

  ssh <<~SH, directory: 'a07'
    #{load_env :cpp}
    make clean
    make THREADS=#{slots}
  SH

  qsub ompexec('./nbody_omp', threads: slots, env: :cpp), problem_size.to_s,
       parallel_environment: 'openmp',
       slots: slots,
       name: 'nbody_omp',
       directory: 'a07'
end
