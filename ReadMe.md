# Parallel Systems

Solutions for the assignments at https://github.com/philippgs/uibk_parsys_19.

## Local Setup

### SSH Access

Passwordless SSH access to LCC2 must be configured as `lcc2`, i.e. `ssh lcc2` should work.

## Remote Setup

### Scratch Symlink

Create a symlink to the scratch partition using

```bash
rake lcc2:symlink_scratch
```

### Homebrew

Install using

```bash
rake lcc2:install_homebrew
```

and activate it with

```bash
eval $(~/.linuxbrew/bin/brew shellenv)
```

### Boost

Install using

```bash
rake lcc2:install_boost
```

and use it in a `Makefile` with

```makefile
CXX_FLAGS += -lboost_mpi -lboost_serialization

ifneq ("$(wildcard $(HOME)/.local/include)", "")
	CXX_FLAGS += -I"$(HOME)/.local/include"
endif

ifneq ("$(wildcard $(HOME)/.local/lib)", "")
	CXX_FLAGS += -L"$(HOME)/.local/lib" -Wl,-rpath,"$(HOME)/.local/lib"
endif
```

### Rust

Install using

```bash
rake lcc2:install_rust
```

and activate it with

```bash
source ~/.cargo/env
```

### OpenMPI 4.0.2

Install using

```bash
rake lcc2:install_openmpi
```

and activate it by activating Homebrew with

```bash
eval $(~/.linuxbrew/bin/brew shellenv)
```
