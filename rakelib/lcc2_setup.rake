task :install_homebrew => :symlink_cache do
  ssh <<~SH
    if ! [[ -d ~/scratch/.linuxbrew ]]; then
      git clone https://github.com/Homebrew/brew ~/scratch/.linuxbrew/Homebrew
    fi

    ln -sfn scratch/.linuxbrew ~/.linuxbrew

    mkdir -p ~/.linuxbrew/bin
    ln -sfn '../Homebrew/bin/brew' ~/.linuxbrew/bin

    set +u
    eval $(~/.linuxbrew/bin/brew shellenv)
    set -u

    brew update
  SH
end

task :install_rust => [:install_homebrew, :symlink_cache] do
  ssh <<~SH
    set +u
    eval $(~/.linuxbrew/bin/brew shellenv)
    set -u

    brew install rustup-init

    mkdir -p ~/scratch/.rustup
    mkdir -p ~/scratch/.cargo
    ln -sfn scratch/.rustup ~/.rustup
    ln -sfn scratch/.cargo ~/.cargo

    rustup-init -y
  SH
end

task :install_openmpi => :install_homebrew do
  ssh <<~SH
    set +u
    eval $(~/.linuxbrew/bin/brew shellenv)
    set -u

    brew install --HEAD open-mpi
  SH
end

task :symlink_cache => :symlink_scratch do
  ssh <<~SH
    rm -rf ~/.cache
    mkdir -p ~/scratch/.cache
    ln -sfn scratch/.cache ~/.cache
  SH
end

task :symlink_scratch do
  ssh <<~SH
    ln -sfn "${SCRATCH}" ~/scratch
  SH
end
