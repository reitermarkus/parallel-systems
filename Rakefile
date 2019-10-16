require 'shellwords'

def windows?
  !(RUBY_PLATFORM !~ /cygwin|mswin|mingw|bccwin|wince|emx/)
end

namespace :a01 do
  task :sync do
    if windows?
      sh 'rclone', 'sync', '-v', 'a01', 'lcc2:a01/'
    else
      sh 'rsync', '-av', '--delete', 'a01/', 'lcc2:a01/'
    end
  end

  def qsub(executable, *args, parallel_environment:, slots:, output_file: 'output.log', error_log: 'error.log', name: nil, sync: true, directory: nil)
    qsub_args = []
    qsub_args << '-cwd' if directory
    qsub_args << '-o' << output_file if output_file
    qsub_args << '-e' << error_log if error_log
    qsub_args << '-pe' << parallel_environment << slots
    qsub_args << '-N' << name if name
    qsub_args << '-sync' << 'yes' if sync
    qsub_args << executable
    qsub_args += args

    script = <<~SH
      set -euo pipefail

      #{directory ? "cd '#{directory}'" : ''}

      trap 'rm -f #{[*output_file, *error_log].shelljoin}' EXIT

      if qsub #{qsub_args.shelljoin}; then
        #{output_file ? "cat #{output_file.shellescape}" : ''}
        exit 0
      else
        exit_status=$?
        #{error_log ? "cat #{error_log.shellescape}" : ''}
        exit $exit_status
      fi
    SH

    sh 'ssh', 'lcc2', script
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
