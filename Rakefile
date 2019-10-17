require 'shellwords'

def windows?
  !(RUBY_PLATFORM !~ /cygwin|mswin|mingw|bccwin|wince|emx/)
end

def sync(directory)
  if windows?
    sh 'rclone', 'sync', '-v', "#{directory}/", "lcc2:scratch/#{directory}/"
  else
    sh 'rsync', '-av', '--exclude', 'target', '--delete', "#{directory}/", "lcc2:scratch/#{directory}/"
  end
end

def ssh(script, directory: nil)
  sh 'ssh', 'lcc2', <<~SH
    set -euo pipefail

    #{directory ? "cd scratch/#{directory.shellescape}" : ''}

    #{script}
  SH
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

  ssh script, directory: directory
end
