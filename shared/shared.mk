MPI_CXX ?= mpic++

CPPFLAGS ?= -O3 -Wall -Wextra -pedantic
CXXFLAGS ?= -std=c++17

ifneq ($(OS), Windows_NT)
  OS ?= $(shell uname -s)
endif

ifeq ($(OS), Darwin)
  ifeq ($(shell brew ls libomp &>/dev/null; echo $$?), 0)
    LIBOMP_PREFIX = $(shell brew --prefix libomp)
    OPENMP_CPPFLAGS += -I$(LIBOMP_PREFIX)/include
    OPENMP_LDFLAGS += -L$(LIBOMP_PREFIX)/lib -lomp
  else
    $(error "OpenMP is not installed, run `brew install libomp`")
  endif
else
  OPENMP_LDFLAGS += -fopenmp
endif

OPENMP_CPPFLAGS += -Xpreprocessor -fopenmp

MPI_LDFLAGS += -lboost_mpi -lboost_serialization

ifneq ("$(wildcard $(HOME)/.local/include)", "")
	MPI_CPPFLAGS += -I"$(HOME)/.local/include"
endif

ifneq ("$(wildcard $(HOME)/.local/lib)", "")
	MPI_LDFLAGS += -L"$(HOME)/.local/lib" -Wl,-rpath,"$(HOME)/.local/lib"
endif

ifeq ($(DEBUG),1)
  CPPFLAGS += -DDEBUG
endif

ifeq ($(MPI_OPTIMIZATION),1)
  CPPFLAGS += -DMPI_OPTIMIZATION
endif

ifeq ($(OPENMP_OPTIMIZATION),1)
  CPPFLAGS += -DOPENMP_OPTIMIZATION
endif

TARGETS ?= $(sort $(patsubst %.cpp,%,$(wildcard *_omp.cpp *_seq.cpp *_mpi.cpp)))

.PHONY: all
all: $(TARGETS)

%_seq: %_seq.o
	$(CXX) $^ $(LDFLAGS) -o $@

%_mpi.o: %_mpi.cpp
	$(MPI_CXX) $(CXXFLAGS) $(CPPFLAGS) $(MPI_CPPFLAGS) -c -o $@ $^

%_mpi: %_mpi.o
	$(MPI_CXX) $^ $(LDFLAGS) $(MPI_LDFLAGS) -o $@

%_mpi_omp.o: %_mpi_omp.cpp
	$(MPI_CXX) $(CXXFLAGS) $(CPPFLAGS) $(MPI_CPPFLAGS) $(OPENMP_CPPFLAGS) -c -o $@ $^

%_mpi_omp: %_mpi_omp.o
	$(MPI_CXX) $^ $(LDFLAGS) $(MPI_LDFLAGS) $(OPENMP_LDFLAGS) -o $@

%_omp.o: %_omp.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OPENMP_CPPFLAGS) -c -o $@ $^

%_omp: %_omp.o
	$(CXX) $^ $(LDFLAGS) $(OPENMP_LDFLAGS) -o $@

.PHONY: clean
clean:
	$(RM) $(TARGETS) *.o

.PHONY: run
run: all
	@separator=''
	@for target in $(TARGETS); do \
		printf "$${separator}"; \
	  echo "$${target}:"; \
		if [[ "$${target}" = *_mpi ]]; then \
		  mpiexec ./$${target}; \
		else \
		  ./$${target}; \
		fi;\
		separator="\n"; \
	done
