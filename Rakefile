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

def load_env(env = nil)
  case env
  when :rust
    <<~SH
      load_homebrew() {
        shopt -q -o nounset
        local nounset=$?

        set +u
        eval $(~/.linuxbrew/bin/brew shellenv)
        [[ $nounset -eq 0 ]] && set -u
      }

      load_homebrew

      source ~/.cargo/env
    SH
  when :cpp
    <<~SH
      module load gcc/8.2.0
      module load openmpi/4.0.1
    SH
  end
end

def mpiexec(executable, env:)
  <<~SH
    #!/usr/bin/env bash
    set -euo pipefail

    #{load_env env}
    mpiexec -n "${NSLOTS}" --mca btl self,vader,tcp #{executable.shellescape} "${@}"
  SH
end

def qsub(executable, *args, mpi_environment: nil, parallel_environment:, slots:, output_file: 'output.log', error_log: 'error.log', name: nil, sync: true, directory: nil)
  qsub_args = []
  qsub_args << '-cwd' if directory
  qsub_args << '-o' << output_file if output_file
  qsub_args << '-e' << error_log if error_log
  qsub_args << '-pe' << parallel_environment << slots
  qsub_args << '-N' << name if name
  qsub_args << '-sync' << 'yes' if sync

  if executable.start_with?('#!')
    qsub_args << '--' unless args.empty?
  else
    qsub_args << executable
  end

  qsub_args += args

  qsub = "qsub #{qsub_args.shelljoin}"

  script = <<~SH
    trap 'rm -f #{[*output_file, *error_log].shelljoin}' EXIT

    exit_status=0
  SH

  script += if executable.start_with?('#!')
    <<~SH
      #{qsub} \<<'EOF' || exit_status=$?
      #{executable}
      EOF
    SH
  else
    <<~SH
      #{qsub} || exit_status=$?
    SH
  end

  script += <<~SH
    if [[ $exit_status -eq 0 ]]; then
      #{output_file ? "cat #{output_file.shellescape}" : ''}
      exit 0
    else
      #{error_log ? "cat #{error_log.shellescape}" : ''}
      exit $exit_status
    fi
  SH

  ssh script, directory: directory
end
