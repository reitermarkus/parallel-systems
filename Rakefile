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

  task :e01 => :sync do
    sh 'ssh', 'lcc2', <<~SH
      cd a01 &&
      qsub -cwd -o output.log -e error.log -pe openmpi-1perhost 8 -N hostname -sync yes hostname.sh &&
      cat output.log || cat error.log
      rm -f *.log
    SH
  end

  task :e02 => :sync do
    [
      { cores: 2, binding: '-binding explicit:0,0:0,1' }, # Same socket, different cores.
      { cores: 2, binding: '-binding explicit:0,0:1,0' }, # Same node, different sockets.
      { cores: 1, binding: '-binding explicit:0,0' },     # Different nodes.
    ].each do |cores:,binding:|
      sh 'ssh', 'lcc2', <<~SH
        cd a01 &&
        qsub -cwd -o output.log -e error.log -pe openmpi-#{cores}perhost 2 #{binding} -N osu -sync yes osu.sh &&
        cat output.log || cat error.log
        rm -f *.log
      SH
    end
  end
end
