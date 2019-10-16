namespace :a01 do
    task :sync => ping('lcc2.uibk.ac.at') do
      sync 'a01'
    end

    task :e01 => :sync do
      qsub 'hostname.sh',
           parallel_environment: 'openmpi-1perhost',
           slots: 8,
           name: 'hostname',
           directory: 'a01'
    end

    task :e02 => :sync do
      [
        { cores: 2, args: %w[--map-by ppr:2:socket] }, # Same socket, different cores.
        { cores: 2, args: %w[--map-by ppr:1:socket] }, # Same node, different sockets.
        { cores: 1, args: %w[--map-by ppr:1:node] }, # Different nodes.
      ].each do |cores:,args:|
        qsub 'osu.sh', *args,
             parallel_environment: "openmpi-#{cores}perhost",
             slots: 2,
             name: 'osu',
             directory: 'a01'
      end
    end
  end
