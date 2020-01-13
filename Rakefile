require 'securerandom'
require 'shellwords'

def windows?
  !(RUBY_PLATFORM !~ /cygwin|mswin|mingw|bccwin|wince|emx/)
end

def sync(directory)
  if windows?
    sh 'rclone', 'sync', '-v', '--exclude', 'target/**', "./shared/", "lcc2:scratch/shared/"
    sh 'rclone', 'sync', '-v', '--exclude', 'target/**', "#{directory}/", "lcc2:scratch/#{directory}/"
  else
    sh 'rsync', '-av', '--exclude', 'target', '--delete', 'shared/', "lcc2:scratch/shared/"
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

def mpiexec(executable, env:, n: nil)
  slots = n ? n : '"${NSLOTS}"'

  <<~SH
    #!/usr/bin/env bash
    set -euo pipefail

    #{load_env env}
    time mpiexec -n #{slots} --mca btl self,vader,tcp #{executable.shellescape} "${@}"
  SH
end

def ompexec(executable, threads: nil, env:)

  <<~SH
    #!/usr/bin/env bash
    set -eou pipefail

    #{load_env env}
    #{threads ? "export OMP_NUM_THREADS='#{threads}'" : ''}
    time #{executable.shellescape} "${@}"
  SH
end

def mpi_ompexec(executable, slots: nil, threads: nil, env:)
  slots ||= '"${NSLOTS}"'

  <<~SH
    #!/usr/bin/env bash
    set -eou pipefail

    #{load_env env}
    #{threads ? "export OMP_NUM_THREADS='#{threads}'" : ''}

    time mpiexec --display-map --display-allocation --map-by node -n #{slots} --mca btl self,vader,tcp #{executable.shellescape} "${@}"
  SH
end

def qsub(executable, *args, parallel_environment:, slots:, output_file: nil, error_log: nil, name: nil, sync: true, directory: nil)
  id = SecureRandom.hex
  current_output_file = "output-#{id}.log"
  current_error_log = "error-#{id}.log"

  qsub_args = []
  qsub_args << '-cwd' if directory
  qsub_args << '-o' << current_output_file
  qsub_args << '-e' << current_error_log
  qsub_args << '-pe' << parallel_environment << slots
  qsub_args << '-N' << name if name
  qsub_args << '-sync' << 'yes' if sync
  qsub_args << '--' unless args.empty?

  unless executable.start_with?('#!')
    executable = <<~SH
      #!/usr/bin/env bash
      time #{executable.shellescape}
    SH
  end

  qsub_args += args

  qsub = "qsub #{qsub_args.shelljoin}"

  script = <<~SH
    trap 'rm #{[current_output_file, current_error_log].shelljoin}' EXIT

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

  if output_file
    script += <<~SH
      cp -f #{[current_output_file, output_file].shelljoin}
    SH
  end

  if error_log
    script += <<~SH
      cp -f #{[current_error_log, error_log].shelljoin}
    SH
  end

  script += <<~SH
    if [[ $exit_status -eq 0 ]]; then
      cat #{current_output_file.shellescape}
      cat #{current_error_log.shellescape}
      exit 0
    else
      cat #{current_error_log.shellescape}
      exit $exit_status
    fi
  SH

  ssh script, directory: directory
end
