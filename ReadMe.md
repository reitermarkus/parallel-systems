# Parallel Systems

Solutions for the assignments at https://github.com/philippgs/uibk_parsys_19.

## Local Setup

### SSH Access

Passwordless SSH access to LCC2 must be configured as `lcc2`, i.e. `ssh lcc2` should work.

## Remote Setup

### Scratch Symlink

Create a symlink to the scratch partition using

```
rake lcc2:symlink_scratch
```

### Homebrew

Install using

```
rake lcc2:install_homebrew
```

and activate it with

```
eval $(~/.linuxbrew/bin/brew shellenv)
```

### Rust

Install using

```
rake lcc2:install_rust
```

and activate it with

```
source ~/.cargo/env
```

### OpenMPI 4.0.2

Install using

```
rake lcc2:install_openmpi
```

and activate it by activating Homebrew with

```
eval $(~/.linuxbrew/bin/brew shellenv)
```
