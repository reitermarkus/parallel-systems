require 'fileutils'
require 'open3'

namespace :docker do
  task :init_cluster do
    stdout, *_ = Open3.capture3('docker', 'inspect', '--format', '{{.State.Running}}', 'docker_master_1')
    container_state = stdout.chomp

    cd './docker' do
      unless File.directory? 'project'
        FileUtils.mkdir_p 'project'
      end

      if container_state == 'false' || container_state.empty?
        sh 'sh', './cluster.sh',  'up',  "size=#{ENV.fetch 'CLUSTER_SIZE', 8 }"
      end
    end
  end

  task :copy_exercises => :init_cluster do
    Dir['./a*'].each { |d|
      FileUtils.copy_entry "#{d}", "./docker/project/#{d}"
    }
  end

  task :exec, [:cmd] => :init_cluster do |_, args|
    cd './docker' do
      sh 'sh', './cluster.sh', 'exec', args[:cmd]
    end
  end

  task :login => :init_cluster do
    cd './docker' do
      sh 'sh', './cluster.sh', 'login'
    end
  end

  task :down => :init_cluster do
    cd './docker' do
      sh 'sh', './cluster.sh', 'down'
    end
  end

  task :scale => :init_cluster do
    cd './docker' do
      sh 'sh', './cluster.sh',  'scale',  "size=#{ENV.fetch 'CLUSTER_SIZE', 8 }"
    end
  end
end
